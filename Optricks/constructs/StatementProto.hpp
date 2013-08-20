/*
 * StatementProto.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef STATEMENTPROTO_HPP_
#define STATEMENTPROTO_HPP_


#include "../containers/settings.hpp"
#include "Stackable.hpp"


#ifndef JUMP_P_
#define JUMP_P_
class Jump;
#endif

#define STATEMENT_P_
class Statement : public Stackable{
	public:
		virtual Value* evaluate(RData& a) = 0;
		virtual Statement* simplify(Jump& jump)  = 0;
		virtual void checkTypes() = 0;
};


class VoidStatement : public Statement{
	public:
		Value* evaluate(RData& a) override;
		Statement* simplify(Jump& jump) override;
		const Token getToken() const override;
		bool operator == (Statement* s) const;
		void write(ostream& a,String r) const override;
		void checkTypes(){}
};

VoidStatement* VOID = new VoidStatement();


#endif /* STATEMENTPROTO_HPP_ */
