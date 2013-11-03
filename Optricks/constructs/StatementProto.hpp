/*
 * StatementProto.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef STATEMENTPROTO_HPP_
#define STATEMENTPROTO_HPP_

#include "Stackable.hpp"

#define STATEMENT_P_
class Statement : public Stackable{
	private:
	public:
		ClassProto* returnType;
		PositionID filePos;
		Statement(PositionID a, ClassProto* rt=NULL) :
			returnType(rt), filePos(a)	{}
		virtual ~Statement(){};
		void error(String s="Compile error", bool exi=true) const{
			filePos.error(s, exi);
		}
		/**
		 * Gets the name of the variable
		 * returns null if not possible
		 */
		virtual String getFullName() =0;
		/**
		 * Stores any return-types into a vector to be analyzed later
		 */
		virtual void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) = 0;
		/**
		 * Creates LLVM IR for all pieces (either by lazy evaluation or immediately).
		 */
		virtual DATA evaluate(RData& a) = 0;
		/**
		 * Simplifies the Statement (if possible)
		 */
		virtual Statement* simplify()  = 0;
		/**
		 * Creates all functions / prototypes
		 */
		virtual void registerClasses(RData& a) = 0;
		/**
		 * Puts the location of the function in memory (creating a prototype)
		 */
		virtual void registerFunctionPrototype(RData& r) = 0;
		/**
		 * Compiles the actual function
		 */
		virtual void buildFunction(RData& r) = 0;
		/**
		 * Determines return-type of expression (and sub-expressions)
		 */
		virtual ClassProto* checkTypes(RData& r) = 0;
		/**
		 * Gets the class which this expression represents (assuming this is a class-type object or reference)
		 */
		virtual ClassProto* getSelfClass(RData& r){
			DATA d = evaluate(r);
			return d.getMyClass(r);
		}
		/**
		 * Gets a variety of pieces of metadata about the expression
		 */
		virtual ReferenceElement* getMetadata(RData& r) = 0;
};


class VoidStatement : public Statement{
	public:
		VoidStatement() : Statement(PositionID(0,0,"#Void")){}
		DATA evaluate(RData& a) override{
			error("Attempted evaluation of void statement");
			return DATA::getNull();
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
		void registerFunctionPrototype(RData& r) override final{};
		void buildFunction(RData& r) override final{};
		ClassProto* checkTypes(RData& r) override final;
		String getFullName() override final{ return "void"; }
		ReferenceElement* getMetadata(RData& r) override final { error("Cannot get ReferenceElement of void"); return NULL; }
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe){
		}
};

static VoidStatement* VOID = new VoidStatement();


#endif /* STATEMENTPROTO_HPP_ */
