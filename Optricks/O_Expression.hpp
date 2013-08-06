/*
 * O_Expression.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses


#ifndef O_EXPRESSION_HPP_
#define O_EXPRESSION_HPP_


bool readExpression(FILE* f,Expression** error);

#include "./O_IO.hpp"
#include "./O_Stream.hpp"
#include "./O_Token.hpp"

#include "primitives/oobject.hpp"

#include "expressions/E_BINOP.hpp"
#include "expressions/E_EOF.hpp"
#include "expressions/E_EXT.hpp"
#include "expressions/E_VAR.hpp"
#include "expressions/E_EOF.hpp"
#include "expressions/E_ARR.hpp"
#include "expressions/E_SEP.hpp"
#include "expressions/E_INDEXER.hpp"
#include "expressions/E_PARENS.hpp"
#include "expressions/E_FUNC_CALL.hpp"
#include "expressions/E_LOOKUP.hpp"


#include "expressions/operators/unary/UO_BNOT.hpp"
#include "expressions/operators/unary/UO_DEREF.hpp"
#include "expressions/operators/unary/UO_NEG.hpp"
#include "expressions/operators/unary/UO_NOT.hpp"
#include "expressions/operators/unary/UO_POS.hpp"
#include "expressions/operators/unary/UO_REF.hpp"


#include "functions/OBJ_FUNC.hpp"
#include "functions/OBJ_KEY_FUNC.hpp"

//TODO
bool readExpression(FILE* f, Expression** exp){
	byte temp;
	if(peekByte(f, &temp)) return true;
	switch(temp){
		case T_LOOKUP:
			*exp = new E_LOOKUP();
			break;
		case T_FUNC_CALL:
			*exp = new E_FUNC_CALL();
			break;
		case T_PARENS:
			*exp = new E_PARENS();
			break;
		case T_EOF:
			*exp = new E_EOF();
			break;
		case T_ARR:
			*exp = new E_ARR();
			break;
		case T_INDEXER:
			*exp = new E_INDEXER();
			break;
		case T_SEP:
			*exp = new E_SEP();
			break;
		case T_INT:
			*exp = new OBJ_INT();
			break;
		case T_STR:
			*exp = new OBJ_STR();
			break;
		case T_DEREF:
			*exp = new UO_DEREF();
			break;
		case T_NEG:
			*exp = new UO_NEG();
			break;
		case T_NOT:
			*exp = new UO_NOT();
			break;
		case T_POS:
			*exp = new UO_POS();
			break;
		case T_REF:
			*exp = new UO_REF();
			break;
		case T_BNOT:
			*exp = new UO_BNOT();
			break;
		case T_DEC:
			*exp = new OBJ_DEC();
			break;
		case T_KEY_FUNC:
			*exp = new OBJ_KEY_FUNC();
			break;
		case T_FUNC:
			*exp = new OBJ_FUNC();
			break;
		case T_O_ARR:
			*exp = new OBJ_ARR();
			break;
	}
	return (*exp)->readBinary(f);
}


#endif  O_EXPRESSION_HPP_ */
