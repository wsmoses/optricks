#ifndef E_PARENS_HPP_
#define E_PARENS_HPP_
#include "../O_Expression.hpp"

class E_PARENS : public Expression{
public:
	Expression* inner;
	E_PARENS(Expression* t) : inner(t) { };
	E_PARENS(){
	};
	~E_PARENS(){
		delete inner;
	};
	bool writeBinary(FILE* f){
		writeByte(f, T_PARENS);
		if(inner->writeBinary(f)) return true;
		return false;
	};
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f, &c)) return true;
		if(c!=T_PARENS ) return true;
		if(readExpression(f, &inner)) return true;
		return false;
	};
	Token getToken(){
		return T_PARENS;
	};
	ostream& write(ostream& f){
		f << "E_PARENS(";
		inner->write(f);
		return f << ")";
	}
};


#endif /* E_PARENS_HPP_ */
