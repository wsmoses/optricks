#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../constructs/Expression.hpp"
#include "../primitives/oarray.hpp"

class E_FUNC_CALL : public Expression{
public:
	Resolvable* toCall;
	ofunction* func;
	std::vector<Expression*> vals;
	E_FUNC_CALL(Resolvable* t, std::vector<Expression*> val) : Expression(objectClass),
			toCall(t), vals(val){ };//TODO analyze return val
	const Token getToken() const override{
		return T_FUNC_CALL;
	};
	void write(ostream& f,String t="") const override{
		f<<"call(" << toCall << ", [";
		bool first = true;
		for(auto& a:vals){
			if(first) first = false;
			else f << ", ";
			a->write(f);
		}
		f<<"])";
	}
	void checkTypes(){
		func = dynamic_cast<ofunction*> (toCall->resolveMeta());
		func->checkTypes();
		if(func->declarations.size() < vals.size()) todo("Function " + ((String) *func)+"called with too many arguments");
		for(unsigned int i = 0; i<vals.size(); i++){
			vals[i]->checkTypes();
			oclass* t = dynamic_cast<oclass*>(func->declarations[i]->classV->pointer->resolveMeta());
			if(t==NULL || vals[i]->returnType!=t)
				todo("Called function with incorrect arguments: needed ",(t==NULL)?"NULL":(t->name),
						" got ", vals[i]->returnType->name);
		}
		for(unsigned int i = vals.size(); i<func->declarations.size(); i++){
			if(func->declarations[i]->value==NULL) todo("Argument "+i, " non-optional");
		}
		returnType = func->postReturnV;
	}
	Expression* simplify() override{
		//Expression* a = toCall->simplify();
		//Expression* b = vals->simplify();
		//if(a->getToken()==T_OOBJECT && b->getToken()==T_OOBJECT && a->returnType==functionClass && b->returnType==arrayClass){
		//	return ((ofunction*)a)->call(((oarray*)b)->data);
		//}
		//else 	return new E_FUNC_CALL(toCall, vals);
		return this;
	}
	Value* evaluate(RData& a) override{
		auto callee = toCall->resolve();//->evaluate(a,context);
		//TODO check that function is created in cases like [ (lambda int int x: x+2)(2) ]
		 std::vector<Value*> Args;
		  for(auto& d: vals){
		    Args.push_back(d->evaluate(a));
		    if (Args.back() == 0) todo("Error in eval of args");
		  }
			for(unsigned int i = Args.size(); i<func->declarations.size(); i++){
				Args.push_back(func->declarations[i]->value->evaluate(a));
			}
		  return a.builder.CreateCall(callee, Args, "calltmp");
	//	todo("Function calls not implemented");
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
