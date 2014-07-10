/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../language/statement/Statement.hpp"
#include "../language/data/ClassFunctionData.hpp"
#include "../operators/LocalFuncs.hpp"

//TODO introduce templates here
class E_LOOKUP :
		public Statement
{
public:
	const Token getToken() const override{ return T_LOOKUP; }
	T_ARGS t_args;
	Statement* left;
	String right;
	virtual ~E_LOOKUP(){};
	PositionID filePos;
	E_LOOKUP(PositionID id, Statement* a,  String b, bool isTemplate):
		Statement(), t_args(isTemplate),
			left(a), right(b),filePos(id){};
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
	}
	void reset() const override final{
		left->reset();
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args,bool isClassMethod)const override final{
		assert(isClassMethod==false);
		const AbstractClass* cla= left->getReturnType();
		if(cla->classType==CLASS_CLASS){
			return left->getMyClass(getRData(), filePos)->staticVariables.getFunctionReturnTypeHere(id,right,t_args, args);
		} else {
			if(cla->hasLocalData(right)){
				const AbstractClass* tmp = cla->getLocalReturnClass(id,right);
				if(tmp->classType==CLASS_FUNC){
					FunctionClass* fc = (FunctionClass*)tmp;
					return fc->returnType;
				} else {
					id.error("Cannot use class '"+tmp->getName()+"' as function");
					exit(1);
				}
			}
			else{
				return getLocalFunctionReturnType(filePos, right, left->getReturnType(), t_args, args);
			}
		}
		auto type=getReturnType();
		if(type->classType==CLASS_FUNC){
			return ((FunctionClass*)type)->returnType;
		}  else if(type->classType==CLASS_LAZY){
			return ((LazyClass*)type)->innerType;
		} else if(type->classType==CLASS_CLASS){
			return type;
		}	else {
			id.error("Class '"+type->getName()+"' cannot be used as function");
			exit(1);
		}
	}

	void registerClasses() const override final{
		left->registerClasses();
	}
	void registerFunctionPrototype(RData& r) const override final{
		left->registerFunctionPrototype(r);
	};
	void buildFunction(RData& r) const override final{
		left->buildFunction(r);
	};

	const AbstractClass* getMyClass(RData& r, PositionID id)const{
		auto t = left->getMyClass(r,id);
		return t->staticVariables.getClassHere(id,right,t_args);
	}
	const AbstractClass* getReturnType() const override final{
		const AbstractClass* superC = left->getReturnType();
		if(superC->classType==CLASS_CLASS){
			return left->getMyClass(getRData(), filePos)->staticVariables.getReturnClassHere(filePos,right, t_args);
		} else {
			return superC->getLocalReturnClass(filePos, right);
		}
	}

	bool hasCastValue(const AbstractClass* const a) const override{
		const AbstractClass* cla = left->getReturnType();
		if(cla->classType==CLASS_CLASS){
			const AbstractClass* c = left->getMyClass(getRData(), filePos);
			return c->staticVariables.getHere(filePos, right, t_args)->hasCastValue(a);
		} else {
			if(cla->hasLocalData(right))
				return cla->getLocalReturnClass(filePos, right)->hasCast(a);
			else if(hasLocalFunction(right, cla)){
				//TODO MAKE WRAPPER FOR ENCLOSED FUNCS
				return false;
			}
			else{
				return false;
			}
		}
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override{
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		const AbstractClass* cla = left->getReturnType();
		if(cla->classType==CLASS_CLASS){
			const AbstractClass* c = left->getMyClass(getRData(), filePos);
			return c->staticVariables.getHere(filePos, right, t_args)->compareValue(a,b);
		} else {
			if(cla->hasLocalData(right))
				return cla->getLocalReturnClass(filePos, right)->compare(a,b);
			else if(hasLocalFunction(right, cla)){
				//TODO MAKE WRAPPER FOR ENCLOSED FUNCS
				assert(0);
				return 0;
			}
			else{
				assert(0);
				return 0;
			}
		}
	}
	const Data* evaluate(RData& a) const override{
		auto eval = left->evaluate(a);
		const AbstractClass* cla = eval->getReturnType();
		///STATIC STUFF
		if(cla->classType==CLASS_CLASS){
			const AbstractClass* c = eval->getMyClass(a, filePos);
			return c->staticVariables.getHere(filePos, right, t_args);
		} else {
			if(cla->hasLocalData(right))
				return cla->getLocalData(a, filePos, right, eval);
			else if(hasLocalFunction(right, cla)){
				return new ClassFunctionData(eval, right, t_args);
			}
			else{
				filePos.error("Could not find local '"+right+"' inside of class "+cla->getName());
				return &VOID_DATA;
			}
		}
	}
};



#endif /* E_LOOKUP_HPP_ */
