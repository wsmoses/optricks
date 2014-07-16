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

		void reset() const override final{
			for(auto& a:vals) if(a) ((const Statement*)a)->reset();
			toCall->reset();
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			assert(isClassMethod==false);
			auto type=getReturnType();
			if(type->classType==CLASS_FUNC){
				return ((FunctionClass*)type)->returnType;
			}  else if(type->classType==CLASS_LAZY){
				return ((LazyClass*)type)->innerType;
			//} else if(type->classType==CLASS_CLASS){
			//	return type;
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
			for(auto &a : vals) if(a) ((const Statement*)a)->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			toCall->registerFunctionPrototype(r);
			for(auto &a : vals) if(a) ((const Statement*)a)->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			toCall->buildFunction(r);
			for(auto &a : vals) if(a) ((const Statement*)a)->buildFunction(r);
		}

		const AbstractClass* getMyClass(RData& r, PositionID id)const{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		const AbstractClass* getReturnType() const override{
			auto FT = toCall->getFunctionReturnType(filePos,vals,false);
			return FT;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{}

		const Data* evaluate(RData& a) const override{
			const Data* tC = toCall->evaluate(a);
			assert(tC);
			auto V= tC->callFunction(a,filePos,vals,nullptr);
			assert(V);
			return V;
		}
};

#endif /* E_FUNC_CALL_HPP_ */
