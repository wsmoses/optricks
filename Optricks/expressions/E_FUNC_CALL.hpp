#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Statement.hpp"
#include "../primitives/ofunction.hpp"

class E_FUNC_CALL : public Statement{
	public:
		Statement* toCall;
		std::vector<Statement*> vals;
		E_FUNC_CALL(PositionID a, Statement* t, std::vector<Statement*> val) : Statement(a),
				toCall(t), vals(val){
		};
		const Token getToken() const override{
			return T_FUNC_CALL;
		};
		void write(ostream& f,String t="") const override{

			toCall->write(f);
			f << "(";
			bool first = true;
			for(auto& a:vals){
				if(first) first = false;
				else f << ", ";
				a->write(f);
			}
			f<<")";
			/*
			f<<"call(";
			toCall->write(f);
			f << ", [";
			bool first = true;
			for(auto& a:vals){
				if(first) first = false;
				else f << ", ";
				a->write(f);
			}
			f<<"])";
			*/
		}
		void registerClasses(RData& r) override final{
			toCall->registerClasses(r);
			for(auto &a : vals) a->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			toCall->registerFunctionArgs(r);
			for(auto &a : vals) a->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			toCall->registerFunctionDefaultArgs();
			for(auto &a : vals) a->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			toCall->resolvePointers();
			for(auto &a : vals) a->resolvePointers();
		}
		ClassProto* checkTypes() override{
			toCall->checkTypes();
			FunctionProto* proto = toCall->getFunctionProto();
			if(proto==NULL) error("Non-existent function prototype");
			if(proto->declarations.size() < vals.size()) error("Function "+proto->name+" called with too many arguments");
			for(unsigned int i = 0; i<vals.size(); i++){
				vals[i]->checkTypes();
				if(proto->declarations[i]->classV->getClassProto()==NULL){
					error("Argument " + proto->declarations[i]->classV->getObjName() + " is not a class FC");
				}
				ClassProto* t = proto->declarations[i]->classV->getClassProto();
				if(t==NULL || ! ( t==autoClass || vals[i]->returnType==autoClass || vals[i]->returnType->hasCast(t) ))
					error("Called function "+proto->name+" with incorrect arguments: needed "+((t==NULL)?"NULL":(t->name))+
							" got "+ vals[i]->returnType->name);
			}
			for(unsigned int i = vals.size(); i<proto->declarations.size(); i++){
				if(proto->declarations[i]->value==NULL) error("Argument "+i, " non-optional");
			}
			return returnType = proto->returnType;
		}
		Statement* simplify() override{
			auto tem = toCall->simplify();
			std::vector<Statement*> g;
			for(auto a:vals) g.push_back(a->simplify());
			return new E_FUNC_CALL(filePos, tem, g);
		}
		Value* evaluate(RData& a) override{
			lambdaFunction* temp = dynamic_cast<lambdaFunction*>(toCall);
			if(temp!=NULL){
				//TODO be aware that this makes something like
				/*
				 * for(int i=0; i<7; i+=1) printi((lambda int z: z*i)(i)) // VALID
				 * yet something like
				 * for(int i=0; i<7; i+=1){
				 *  auto tmp = (lambda int z: z*i)
				 *  printi(tmp(i))
				 *  } 													 // INVALID
				 */
				//for(int i=0; i<7; i+=1){ auto tmp = (lambda int z: z*z) printi(tmp(i)) }
				for(unsigned int i = 0; i<temp->prototype->declarations.size(); i++){
					Declaration* decl = temp->prototype->declarations[i];
					if(i<vals.size()) decl = new Declaration(decl->filePos, decl->classV, decl->variable, vals[i]);
					else if(decl->value==NULL || decl->value->getToken()==T_VOID) error("No argument for lambda function!");
					decl->evaluate(a);
				}
				return temp->ret->evaluate(a);
			}
			Value* callee = toCall->evaluate(a);
			FunctionProto* proto = toCall->getFunctionProto();
			std::vector<Value*> Args;

			for(unsigned int i = 0; i<vals.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getClassProto();
				Args.push_back(vals[i]->returnType->castTo(a, vals[i]->evaluate(a), t));
				if (Args.back() == 0) error("Error in eval of args");
			}
			for(unsigned int i = Args.size(); i<proto->declarations.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getClassProto();
				Args.push_back( proto->declarations[i]->value->returnType->castTo(a, proto->declarations[i]->value->evaluate(a), t));
			}
			if(returnType==voidClass) return a.builder.CreateCall(callee, Args);
			else return a.builder.CreateCall(callee, Args, "calltmp");
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); return NULL;}
};


#endif /* E_FUNC_CALL_HPP_ */
