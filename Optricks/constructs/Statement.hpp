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
	ClassProto* getSelfClass(RData& r) override { error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
	String getFullName() override{ error("Cannot get full name of construct "+str<Token>(getToken())); return ""; }
	ReferenceElement* getMetadata(RData& r) override{ error("Cannot get ReferenceElement of construct "+str<Token>(getToken())); return NULL; }
};
class ClassProtoWrapper : public Construct{
	public:
		ClassProto* cp;
		ReferenceElement* getMetadata(RData& r) override final{
			//TODO make resolvable for class with static-functions / constructors
			return new ReferenceElement("",NULL,cp->name, DATA::getClass(cp), funcMap());
		}
		ClassProto* getSelfClass(RData& r) override{
			return cp;
		}
		ClassProtoWrapper(ClassProto* c) : Construct(PositionID(0,0,"#classWrapper"),classClass),cp(c){
			assert(cp!=NULL);
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
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
			a << cp->name;
		}
		DATA evaluate(RData& r){
			return DATA::getClass(cp);
		}
		String getFullName() override final{
			return cp->name;
		}
		void registerClasses(RData& r) override final{
		}
		void registerFunctionPrototype(RData& r) override final{
		}
		void buildFunction(RData& r) override final{
		}
};
#endif /* STATEMENT_HPP_ */
