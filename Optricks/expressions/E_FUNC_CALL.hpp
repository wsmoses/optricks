#ifndef E_FUNC_CALL_HPP_
#define E_FUNC_CALL_HPP_
#include "../O_Expression.hpp"

class E_FUNC_CALL : public Expression{
public:
	Expression* toCall;
	E_ARR* vals;
	E_FUNC_CALL(Expression* t, E_ARR* val) : toCall(t), vals(val){ };
	E_FUNC_CALL(){
	};
	~E_FUNC_CALL(){
		delete toCall;
		delete vals;
	};
	bool writeBinary(FILE* f){
		writeByte(f, T_FUNC_CALL);
		if(toCall->writeBinary(f)) return true;
		if(vals->writeBinary(f)) return true;
		return false;
	};
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f, &c)) return true;
		if(c!=T_FUNC_CALL ) return true;
		if(readExpression(f, &toCall)) return true;
		Expression* e;
		if(readExpression(f, &e)) return true;
		if(e->getToken()!=T_ARR) return true;
		vals = (E_ARR*)e;

		return false;
	};
	Token getToken(){
		return T_FUNC_CALL;
	};
	ostream& write(ostream& f){
		f << "E_FUNC_CALL(";
		toCall->write(f);
		f << ", ";
		vals->write(f);
		return f << ")";
	}
};


#endif /* E_FUNC_CALL_HPP_ */
