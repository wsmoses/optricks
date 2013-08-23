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

		AllocaInst* getAlloc() override final{ return NULL; };
		void write(ostream& f,String t="") const override{
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
		}
		FunctionProto* getFunctionProto() override final{
			error("E_FUNC_CALL getFunctionProto() not implemented");
			return NULL;
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
			if(proto==NULL) error("Non-existant function prototype");
			if(proto->declarations.size() < vals.size()) error("Function "+proto->name+" called with too many arguments");
			for(unsigned int i = 0; i<vals.size(); i++){
				vals[i]->checkTypes();
				if(proto->declarations[i]->classV->pointer->resolveReturnClass()!=classClass){
					error("Argument " + proto->declarations[i]->classV->pointer->name + "is not a class");
				}
				ClassProto* t = proto->declarations[i]->classV->pointer->resolveSelfClass();
				if(t==NULL || vals[i]->returnType!=t)
					error("Called function with incorrect arguments: needed "+((t==NULL)?"NULL":(t->name))+
							" got "+ vals[i]->returnType->name);
			}
			for(unsigned int i = vals.size(); i<proto->declarations.size(); i++){
				if(proto->declarations[i]->value==NULL) error("Argument "+i, " non-optional");
			}
			return returnType = proto->returnType;
		}
		Statement* simplify() override{
			return new E_FUNC_CALL(filePos, toCall->simplify(), vals);
		}
		Value* evaluate(RData& a) override{
			Value* callee = toCall->evaluate(a);
			FunctionProto* proto = toCall->getFunctionProto();
			//TODO check that function is created in cases like [ (lambda int int x: x+2)(2) ]
			std::vector<Value*> Args;
			for(auto& d: vals){
				Args.push_back(d->evaluate(a));
				if (Args.back() == 0) error("Error in eval of args");
			}
			for(unsigned int i = Args.size(); i<proto->declarations.size(); i++){
				Args.push_back(proto->declarations[i]->value->evaluate(a));
			}
			if(returnType==voidClass) return a.builder.CreateCall(callee, Args);
			else return a.builder.CreateCall(callee, Args, "calltmp");
		}
};


#endif /* E_FUNC_CALL_HPP_ */
