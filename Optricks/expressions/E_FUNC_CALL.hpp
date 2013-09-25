#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Statement.hpp"
#include "../primitives/ofunction.hpp"

class E_FUNC_CALL : public Statement{
	public:
		Statement* toCall;
		std::vector<Statement*> vals;
		virtual ~E_FUNC_CALL(){};
		E_FUNC_CALL(PositionID a, Statement* t, std::vector<Statement*> val) : Statement(a,NULL),
				toCall(t), vals(val){
		};
		FunctionProto* generateFunctionProto(RData& r) const{
			std::vector<Declaration*> dec;
			for(auto& a:vals){
				auto cp=a->checkTypes(r);
				if(cp==NULL) error("TYPE IS NULL!!");
				ClassProtoWrapper* cpw = new ClassProtoWrapper(cp);
				E_VAR* var = new E_VAR(filePos,cpw->getMetadata(r));
				dec.push_back(new Declaration(filePos,cpw,var,NULL));
			}
			return new FunctionProto(toCall->getFullName(),dec,NULL);
		}
		ReferenceElement* getMetadata(RData& r) override final {
			error("getting metadata of func-call");
			return NULL;
		}
		Value* getLocation(RData& a) override final{
			//TODO
			return NULL;
		}
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
		String getFullName() override final{
			error("Cannot get full name of func_call");
			return "";
		}
		ClassProto* checkTypes(RData& r) override{
			toCall->checkTypes(r);
			if(toCall->returnType==classClass){
				for(unsigned int i = 0; i<vals.size(); i++){
								vals[i]->checkTypes(r);
				}
				return returnType = toCall->getMetadata(r)->selfClass;
			} //TODO oh -- constructor
			FunctionProto* proto = toCall->getMetadata(r)->funcs.get(generateFunctionProto(r),filePos,r).second;
			if(proto==NULL) error("Non-existent function prototype");
			return returnType = proto->returnType;
		}
		Statement* simplify() override{
			auto tem = toCall->simplify();
			std::vector<Statement*> g;
			for(auto a:vals) g.push_back(a->simplify());
			return new E_FUNC_CALL(filePos, tem, g);
		}
//		Value* getLocation(RData& a) override{
	//TODO make data a struct {Value (data), Value (location) }
//		}
		DATA evaluate(RData& a) override{
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
			if(toCall->returnType==classClass){
				return toCall->getMetadata(a)->selfClass->construct(a,vals,filePos);
			}
			if(toCall->returnType==classClass){
					return toCall->getMetadata(a)->selfClass->construct(a,vals,filePos);
			}
			auto funcs = toCall->getMetadata(a)->funcs.get(generateFunctionProto(a),filePos,a);
			FunctionProto* proto = funcs.second;
			Value* callee = funcs.first;
//			Value* callee = toCall->evaluate(a);
			std::vector<Value*> Args;

			for(unsigned int i = 0; i<vals.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getMetadata(a)->selfClass;
				Args.push_back(vals[i]->returnType->castTo(a, vals[i]->evaluate(a), t));
				if (Args.back() == 0) error("Error in eval of args");
			}
			for(unsigned int i = Args.size(); i<proto->declarations.size(); i++){
				ClassProto* t = proto->declarations[i]->classV->getMetadata(a)->selfClass;
				Args.push_back( proto->declarations[i]->value->returnType->castTo(a, proto->declarations[i]->value->evaluate(a), t));
			}
			if(returnType==voidClass) return a.builder.CreateCall(callee, Args);
			else return a.builder.CreateCall(callee, Args, "calltmp");
		}
};


#endif /* E_FUNC_CALL_HPP_ */
