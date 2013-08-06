/*
 * Jump.hpp
 *
 *  Created on: Jul 29, 2013
 *      Author: wmoses
 */

#ifndef JUMP_HPP_
#define JUMP_HPP_

enum JumpType{
	NJUMP = 0,
	RETURN = 1,
	BREAK = 2,
	CONTINUE = 3
};

#include "../containers/settings.hpp"


#include "./Expression.hpp"
//#ifndef EXPRESSION_P_
//#define EXPRESSION_P_
//class Expression;
//#endif

#ifndef OCLASS_P_
#define OCLASS_P_
class oclass;
#endif

#include "../primitives/oobject.hpp"

#define JUMP_P_
class Jump{
	public:
		JumpType type;
		String label;
		Expression* ret;
		Jump(JumpType j=NJUMP, String lab="",Expression* r=NULL);
		bool operator == (Jump& a);
};

Jump NOJUMP(NJUMP,"");

Jump::Jump(JumpType j, String lab,Expression* r): type(j), label(lab),ret((r==NULL)?NULLV:r){

}
bool Jump::operator == (Jump& a){
	return type==a.type && label==a.label && ret==a.ret;
}
#endif /* JUMP_HPP_ */
