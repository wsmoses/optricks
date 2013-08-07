/*
 * Expression.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef EXPRESSION_HPP_
#define EXPRESSION_HPP_

//#include "../O_TOKEN.hpp"

#ifndef OOBJECT_P_
#define OOBJECT_P_
class oobject;
#endif

#ifndef OCLASS_P_
#define OCLASS_P_
class oclass;
#endif

#include "StatementProto.hpp"

#ifndef JUMP_P_
#define JUMP_P_
class Jump;
#endif

#define EXPRESSION_P_
class Expression : public Statement{
	public:
		oclass* const returnType;
		Expression(oclass* c) : returnType(c){}
		virtual oobject* evaluate() = 0;
		oobject* evaluate(Jump& j) override final;
		virtual Expression* simplify() = 0;
		Expression* simplify(Jump& jump) override final;
};


#include "Jump.hpp"
#include "Statement.hpp"
#include "../primitives/oobjectproto.hpp"

oobject* Expression::evaluate(Jump& j){
	j = NOJUMP;
	auto temp = evaluate();
	return temp;
}

Expression* Expression::simplify(Jump& jump){
	jump = NOJUMP;
	return simplify();
}
#endif /* EXPRESSION_HPP_ */
