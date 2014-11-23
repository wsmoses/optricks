/*
 * ClassLib.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: Billy
 */

#ifndef CLASSLIB_HPP_
#define CLASSLIB_HPP_

#include "./AbstractClass.hpp"
#include "./UserClass.hpp"
#include "./builtin/NamedTupleClass.hpp"
#include "./builtin/TupleClass.hpp"
#include "./builtin/IntClass.hpp"
#include "./builtin/BigIntClass.hpp"
#include "./builtin/FloatClass.hpp"
#include "./literal/IntLiteralClass.hpp"

const AbstractClass* getMin(const AbstractClass* a, const AbstractClass* b, PositionID id){
	if(a==b) return a;
	if(a->classType==CLASS_MATHLITERAL){
		if(a->hasCast(b)) return b;
	}
	if(b->classType==CLASS_MATHLITERAL){
		if(b->hasCast(a)) return a;
	}
	if(a->classType==CLASS_COMPLEX){
		if(b->classType==CLASS_COMPLEX) return ComplexClass::get((const RealClass*)
				getMin(((ComplexClass*)a)->innerClass,((ComplexClass*)b)->innerClass, id) );
		else return ComplexClass::get((const RealClass*)
				getMin(((ComplexClass*)a)->innerClass,b, id) );
	} else if(a->classType==CLASS_INT){
		if(b->classType==CLASS_INTLITERAL) return a;
		else if(b->classType==CLASS_BIGINT) return b;
		else if(b->classType==CLASS_FLOAT) return b;
		else if(b->classType==CLASS_INT){
			auto at = ((const IntClass*)a)->getWidth();
			auto bt = ((const IntClass*)b)->getWidth();
			if(at<=bt) return a;
			else return b;
		} else if(b->classType==CLASS_COMPLEX){
			return ComplexClass::get((const RealClass*)getMin(a, ((const ComplexClass*)b)->innerClass, id));
		}
		else{
			id.compilerError("GetMin 2-1a has not been implemented");
			exit(1);
		}
	} else if(a->classType==CLASS_BIGINT){
		if(b->classType==CLASS_INTLITERAL) return a;
		else if(b->classType==CLASS_INT) return a;
		else if(b->classType==CLASS_BIGINT) return a;
		else if(b->classType==CLASS_COMPLEX){
			return ComplexClass::get((const RealClass*)getMin(a, ((const ComplexClass*)b)->innerClass, id));
		}
		else{
			id.compilerError("GetMin 2-1a has not been implemented");
			exit(1);
		}
	} else if(a->classType==CLASS_FLOAT){
		if(b->classType==CLASS_INTLITERAL) return a;
		else if(b->classType==CLASS_FLOATLITERAL) return a;
		else if(b->classType==CLASS_INT) return a;
		else if(b->classType==CLASS_FLOAT){
			auto at = ((const FloatClass*)a)->getWidth();
			auto bt = ((const FloatClass*)b)->getWidth();
			if(at<=bt) return a;
			else return b;
		} else if(b->classType==CLASS_COMPLEX){
			return ComplexClass::get((const RealClass*)getMin(a, ((const ComplexClass*)b)->innerClass, id));
		}
		else{
			id.compilerError("GetMin 2-1b has not been implemented");
			exit(1);
		}
	} else if(a->classType==CLASS_INTLITERAL){
		if(b->classType==CLASS_FLOATLITERAL) return b;
		else if(b->classType==CLASS_INT) return b;
		else if(b->classType==CLASS_BIGINT) return b;
		else if(b->classType==CLASS_FLOAT) return b;
		else if(b->classType==CLASS_COMPLEX){
			return ComplexClass::get((const RealClass*)getMin(a, ((const ComplexClass*)b)->innerClass, id));
		}
		else{
			id.compilerError("GetMin 2-1c has not been implemented");
			exit(1);
		}
	} else if(a->classType==CLASS_FLOATLITERAL){
		if(b->classType==CLASS_FLOATLITERAL) return a;
		else if(b->classType==CLASS_INTLITERAL) return a;
		else if(b->classType==CLASS_FLOAT) return b;
		else if(b->classType==CLASS_COMPLEX){
			return ComplexClass::get((const RealClass*)getMin(a, ((const ComplexClass*)b)->innerClass, id));
		}
		else{
			id.compilerError("GetMin 2-1d has not been implemented");
			exit(1);
		}
	} else if(a->classType==CLASS_TUPLE || a->classType==CLASS_NAMED_TUPLE){
		const TupleClass* ta = (const TupleClass*)a;
		const TupleClass* tb = (const TupleClass*)b;
		if(ta->innerTypes.size()!=tb->innerTypes.size()){
			id.error("No valid minimum return type of "+ta->getName()+" "+tb->getName());
			exit(1);
		}
		std::vector<const AbstractClass*> mc;
		for(unsigned i=0; i<ta->innerTypes.size(); i++){
			mc.push_back(getMin(ta->innerTypes[i],tb->innerTypes[i],id));
		}
		if(b->classType==CLASS_TUPLE && a->classType==CLASS_TUPLE){
			return TupleClass::get(mc);
		} else if(b->classType==CLASS_NAMED_TUPLE && a->classType==CLASS_NAMED_TUPLE){
			auto nta = (const NamedTupleClass*)a;
			auto ntb = (const NamedTupleClass*)b;
			for(unsigned i=0; i<ta->innerTypes.size(); i++){
				if(nta->innerNames[i]!=ntb->innerNames[i]){
					id.error("No valid minimum return type of "+ta->getName()+" "+tb->getName());
					exit(1);
				}
			}
			return NamedTupleClass::get(mc, nta->innerNames);
		} else {
			if(b->classType==CLASS_NAMED_TUPLE)
				return NamedTupleClass::get(mc, ((const NamedTupleClass*)b)->innerNames);
			else
				return NamedTupleClass::get(mc, ((const NamedTupleClass*)a)->innerNames);
		}
	}
	id.compilerError("GetMin 2 has not been implemented "+a->getName()+" "+b->getName());
	exit(1);
}

/*
const AbstractClass* getMin(const std::vector<const AbstractClass*>& ac, PositionID id){
	if(ac.size()==1) return ac[0];
	else if(ac.size()==2) return getMin(ac[0],ac[1],id);
	id.compilerError("GetMin inf has not been implemented "+ str(ac.size()));
	exit(1);
}*/
#endif /* CLASSLIB_HPP_ */
