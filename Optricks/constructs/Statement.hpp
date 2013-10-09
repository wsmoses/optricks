/*
 * Statement.hpp
 *
 *  Created on: Jul 27, 2013
 *      Author: wmoses
 */

#ifndef STATEMENT_HPP_
#define STATEMENT_HPP_

#include "StatementProto.hpp"

ClassProto* VoidStatement::checkTypes(RData& r){
	return returnType = voidClass;
}

class Construct : public Statement{
public:
	virtual ~Construct(){};
	Construct(PositionID a, ClassProto* pr) : Statement(a, pr){};
	ClassProto* getSelfClass() override { error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
	String getFullName() override{ error("Cannot get full name of construct "+str<Token>(getToken())); return ""; }
	ReferenceElement* getMetadata(RData& r) override{ error("Cannot get ReferenceElement of construct "+str<Token>(getToken())); return NULL; }
	Constant* getConstant(RData& r) override final{ return NULL; }
};
class ClassProtoWrapper : public Construct{
	public:
		ClassProto* cp;
		ReferenceElement* getMetadata(RData& r) override final{
			//TODO make resolvable for class with class-functions / constructors
			return new ReferenceElement("",NULL,cp->name, NULL, classClass, funcMap(), cp, NULL);
		}
		ClassProto* getSelfClass() override{
			return cp;
		}
		ClassProtoWrapper(ClassProto* c) : Construct(PositionID(),classClass){
			cp = c;
		}
		ClassProto* checkTypes(RData& r) override final{
			return returnType;
		}
		ClassProtoWrapper* simplify() override final{
			return this;
		}
		const Token getToken() const override final{
			return T_CLASSPROTO;
		}
		void write(ostream& a, String s="") const{
			error("Cannot write classProto Wrapper");
		}
		DATA evaluate(RData& r){
			error("Can't eval :(");
			return NULL;
		}
		String getFullName() override final{
			return cp->name;
		}
		void registerClasses(RData& r) override final{
		}
		void registerFunctionArgs(RData& r) override final{
		}
		void registerFunctionDefaultArgs() override final{
		}
		void resolvePointers() override final{
		}
};
#endif /* STATEMENT_HPP_ */
