/*
 * LiteralClass.hpp
 *
 *  Created on: Dec 23, 2013
 *      Author: Billy
 */

#ifndef LITERALCLASS_HPP_
#define LITERALCLASS_HPP_

#include "../AbstractClass.hpp"
/*
class LiteralClass: public AbstractClass{
public:
	~LiteralClass(){};
	bool hasCast(const AbstractClass* toCast) const override{
		PositionID(0,0,"<tmp>").compilerError("Cannot hasCast of LiteralClass");
		exit(1);
	}
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}
	bool noopCast(const AbstractClass* toCast) const override{
		PositionID(0,0,"<tmp>").compilerError("Cannot hasCast of LiteralClass");
		exit(1);
	}
	int compare(const AbstractClass* a, const AbstractClass* b) const override final{
		PositionID(0,0,"<tmp>").compilerError("Cannot compare of LiteralClass");
		exit(1);
	}
	Value* castTo(const AbstractClass* toCast, RData& r, PositionID id, Value* valueToCast) const override{
		id.error("Internal Compiler Error - Cannot castTo of LiteralClass");
		exit(1);
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		id.error("Internal Compiler Error - Cannot getLocalReturnClass of LiteralClass");
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		id.error("Internal Compiler Error - Cannot getLocalData of LiteralClass");
		exit(1);
	}
	LiteralClass(String n, ClassType ct):AbstractClass(nullptr,n, nullptr,LITERAL_LAYOUT,ct,true,nullptr){};
};

LiteralClass* intLiteralClass = new LiteralClass("#intLiteral",CLASS_INT);
LiteralClass* floatLiteralClass = new LiteralClass("#floatLiteral",CLASS_FLOAT);
LiteralClass* complexLiteralClass = new LiteralClass("#complexLiteral",CLASS_COMPLEX);

LiteralClass* stringLiteralClass = new LiteralClass("#stringLiteral",CLASS_STR);
//LiteralClass* charLiteralClass = new LiteralClass("#charLiteral",CLASS_CHAR);
//LiteralClass* rationalLiteralClass = new LiteralClass("#rationalLiteral",CLASS_RATIONAL);
LiteralClass* tupleLiteralClass = new LiteralClass("#tupleLiteral",CLASS_TUPLE);
LiteralClass* namedTupleLiteralClass = new LiteralClass("#namedTupleLiteral",CLASS_NAMED_TUPLE);
LiteralClass* arrayLiteralClass = new LiteralClass("#arrayLiteral",CLASS_ARRAY);
LiteralClass* mapLiteralClass = new LiteralClass("#mapLiteral",CLASS_MAP);
LiteralClass* setLiteralClass = new LiteralClass("#setLiteral",CLASS_SET);
LiteralClass* vectorLiteralClass = new LiteralClass("#vectorLiteral",CLASS_VECTOR);
LiteralClass* nullLiteralClass = new LiteralClass("#nullLiteral",CLASS_NULL);

*/


#endif /* LITERALCLASS_HPP_ */
