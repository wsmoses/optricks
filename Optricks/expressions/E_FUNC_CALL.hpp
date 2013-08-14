#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Expression.hpp"
#include "../primitives/oarray.hpp"

class E_FUNC_CALL : public Expression{
public:
	Expression* toCall;
	Expression* vals;
	E_FUNC_CALL(Expression* t, Expression* val) : Expression(objectClass),
			toCall(t), vals(val){ };//TODO analyze return val
	const Token getToken() const override{
		return T_FUNC_CALL;
	};
	void write(ostream& f,String t="") const override{
		f<<"call(" << toCall << ", "<<vals<<")";
	}
	void checkTypes(){
		toCall->checkTypes();
		todo("Function call type-checking");
	}
	Expression* simplify() override{
		Expression* a = toCall->simplify();
		Expression* b = vals->simplify();
		if(a->getToken()==T_OOBJECT && b->getToken()==T_OOBJECT && a->returnType==functionClass && b->returnType==arrayClass){
			return ((ofunction*)a)->call(((oarray*)b)->data);
		}
		else return new E_FUNC_CALL(a,b);
	}
	Value* evaluate(RData& a,LLVMContext& context) override{
		/*auto callee = toCall->evaluate(a,context);

		 std::vector<Value*> ArgsV;
		  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		    ArgsV.push_back(Args[i]->Codegen());
		    if (ArgsV.back() == 0) return 0;
		  }

		  return Builder.CreateCall(CalleeF, ArgsV, "calltmp");*/
		todo("Function calls not implemented");
		/*
		oobject* a = toCall->evaluate();
		oobject* b = vals->evaluate();
		if(a->returnType==functionClass && b->returnType==arrayClass){
			return ((ofunction*)a)->call(((oarray*)b)->data);
		}
		else{
			cerr << "Invalid types for function call";
			exit(0);
			return 0;
		}
		*/
	}
};


#endif /* E_FUNC_CALL_HPP_ */
