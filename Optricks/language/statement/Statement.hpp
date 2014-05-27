/*
 * Statement.hpp
 *
 *  Created on: Jan 7, 2014
 *      Author: Billy
 */

#ifndef STATEMENT_HPP_
#define STATEMENT_HPP_
#include "Token.hpp"
#include "../data/Data.hpp"
#include "../module/Scopable.hpp"
#include "../evaluatable/Evaluatable.hpp"
#define STATEMENT_P_
class Statement : public Evaluatable{
	private:
	public:
		virtual ~Statement(){};
		bool hasCastValue(const AbstractClass* const a) const override ;
		int compareValue(const AbstractClass* const a, const AbstractClass* const b) const;
		/**
		 * Stores any return-types into a vector to be analyzed later
		 */
		virtual void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) = 0;
		/**
		 * Creates all functions / prototypes
		 */
		virtual void registerClasses() const = 0;
		/**
		 * Puts the location of the function in memory (creating a prototype)
		 */
		virtual void registerFunctionPrototype(RData& r) const = 0;
		/**
		 * Compiles the actual function
		 */
		virtual void buildFunction(RData& r) const = 0;
		/**
		 * Determines return-type of expression (and sub-expressions)
		 */
		//virtual AbstractClass* getReturnType() const =0;
		//virtual AbstractClass* checkTypes() = 0;
		/**
		 * Gets the class which this expression represents (assuming this is a class-type object or reference)
		 */
		virtual const AbstractClass* getSelfClass(PositionID id){
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);}; /*(RData& r){
			return evaluate(r).getMyClass(r);
		}*/
		llvm::Value* evalCastV(RData& r,const AbstractClass* c, PositionID id);
		Statement(){};
		virtual const Token getToken() const = 0;
		//virtual bool hasCastValue(AbstractClass* a)=0;
};

class ErrorStatement: public Statement{
public:
	PositionID filePos;
	ErrorStatement(PositionID a) :
		filePos(a)	{}
	virtual ~ErrorStatement(){};
	void error(String s="Compile error", bool exi=true) const{
		filePos.error(s, exi);
	}
};

class VoidStatement : public Statement{
	public:
		VoidStatement() : Statement(){}
		const Data* evaluate(RData& a) const override{
			PositionID(0,0,"#Void").error("Attempted evaluation of void statement");
			exit(1);
		}
		const Token getToken() const override{
			return T_VOID;
		}
		bool operator == (Statement* s) const{
			return s->getToken()==T_VOID;
		}
		//bool hasCastValue(AbstractClass* a){
		//	return a->classType==CLASS_VOID;
		//}
		void registerClasses() const override final{}
		void registerFunctionPrototype(RData& r) const override final{};
		void buildFunction(RData& r) const override final{};
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b) const override final{
			id.compilerError("Getting return type of voidType");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			PositionID(0,0,"<tmp>").compilerError("Getting type of voidType");
			exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{}
};

static VoidStatement* VOID_STATEMENT = new VoidStatement();

class VariableReference : public Statement{
	public:
		VariableReference():Statement(){};
		/**
		 * Gets a variety of pieces of metadata about the expression
		 */
		//virtual Resolvable getMetadata()=0;
		/**
		 * Gets the name of the variable
		 */
//		virtual String getFullName() =0;
		virtual String getShortName() const=0;
};




#endif /* STATEMENT_HPP_ */
