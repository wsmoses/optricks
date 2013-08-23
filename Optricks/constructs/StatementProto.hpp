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

#define STATEMENT_P_
class Statement : public Stackable{
	public:
		ClassProto* returnType;
		PositionID filePos;
		Statement(PositionID a, ClassProto* rt=NULL) :
			returnType(rt), filePos(a)
		{}
		void error(String s="Compile error", bool exi=true){
			cerr << s << " in ";
			cerr << filePos.fileName;
			cerr << " line:";
			cerr << filePos.lineN;
			cerr << ", char: ";
			cerr << filePos.charN;
			cerr << endl << flush;
			if(exi) exit(0);
		}
		virtual Value* evaluate(RData& a) = 0;
		virtual Statement* simplify()  = 0;
		virtual void registerClasses(RData& a) = 0;
		virtual void registerFunctionArgs(RData& a) = 0;
		virtual void registerFunctionDefaultArgs() = 0;
		virtual void resolvePointers() = 0;
		virtual ClassProto* checkTypes() = 0;
		virtual FunctionProto* getFunctionProto() = 0;
		virtual AllocaInst* getAlloc() = 0;
};


class VoidStatement : public Statement{
	public:
		VoidStatement() : Statement(PositionID(0,0,"#Void")){}
		Value* evaluate(RData& a) override{
			error("Attempted evaluation of void statement");
			return NULL;
		}
		Statement* simplify() override final{
			return this;
		}
		const Token getToken() const override{
			return T_VOID;
		}
		bool operator == (Statement* s) const{
			return s->getToken()==T_VOID;
		}
		void write(ostream& a,String r="") const override{
			a << "void";
		}
		void registerClasses(RData& r) override final{}
		void registerFunctionArgs(RData& r) override final{};
		void registerFunctionDefaultArgs() override final{};
		void resolvePointers() override final{};
		FunctionProto* getFunctionProto() override final{ return NULL; };
		AllocaInst* getAlloc() override final{ return NULL; };
		ClassProto* checkTypes() override final;
};

static VoidStatement* VOID = new VoidStatement();


#endif /* STATEMENTPROTO_HPP_ */
