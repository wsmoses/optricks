#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../language/statement/Statement.hpp"
#include "../language/function/AbstractFunction.hpp"
#include "../language/data/Data.hpp"

#define E_FUNC_CALL_C_
//TODO if calling variables are all constants -- inline the function call
//TODO make constructors into generators
class E_FUNC_CALL : public ErrorStatement{
	public:
		Statement* toCall;
		const std::vector<const Evaluatable*> vals;
		virtual ~E_FUNC_CALL(){};
		E_FUNC_CALL(PositionID a, Statement* t, const std::vector<const Evaluatable*>& val) : ErrorStatement(a),
				toCall(t), vals(val){
		};
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			auto type=getReturnType();
			if(type->classType==CLASS_FUNC){
				return ((FunctionClass*)type)->returnType;
			}  else if(type->classType==CLASS_CLASS){
				return type;
			}	else {
				id.error("Class '"+type->getName()+"' cannot be used as function");
				exit(1);
			}
		}
		const Token getToken() const override{
			return T_FUNC_CALL;
		};
		void registerClasses() const override final{
			toCall->registerClasses();
			for(auto &a : vals) ((const Statement*)a)->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			toCall->registerFunctionPrototype(r);
			for(auto &a : vals) ((const Statement*)a)->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			toCall->buildFunction(r);
			for(auto &a : vals) ((const Statement*)a)->buildFunction(r);
		}
		const AbstractClass* getReturnType() const override{
			return toCall->getFunctionReturnType(filePos,vals);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{}
		const Data* evaluate(RData& a) const override{
			const Data* tC = toCall->evaluate(a);
			assert(tC);
			assert(tC->type);
			return tC->callFunction(a,filePos,vals);
		}
};

#endif /* E_FUNC_CALL_HPP_ */
