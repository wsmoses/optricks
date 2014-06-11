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
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args,bool isClassMethod)const override final{
		assert(isClassMethod==false);
		const AbstractClass* cla= left->getReturnType();
		if(cla->classType==CLASS_CLASS){
			return left->getMyClass(getRData(), filePos)->staticVariables.getFunctionReturnType(id,right,t_args, args);
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
		return t->staticVariables.getClass(id,right,t_args);
	}
	const AbstractClass* getReturnType() const override final{
		const AbstractClass* superC = left->getReturnType();
		if(superC->classType==CLASS_CLASS){
			return left->getMyClass(getRData(), filePos)->staticVariables.getReturnClass(filePos,right, t_args);
		} else {
			return superC->getLocalReturnClass(filePos, right);
		}
	}
	const Data* evaluate(RData& a) const override{
		auto eval = left->evaluate(a);
		const AbstractClass* cla = eval->getReturnType();
		///STATIC STUFF
		if(cla->classType==CLASS_CLASS){
			const AbstractClass* c = eval->getMyClass(a, filePos);
			/*if(t_args.inUse>0){
				filePos.warning("Using only class templates");
				return c->staticVariables.getClass(filePos, right, t_args);
			}*/
			return c->staticVariables.get(filePos, right, t_args);
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
		/*
			SCOPE_TYPE varType = lT->getScopeType(filePos, right);
			switch(varType){
			case SCOPE_STATIC_FUNC:
			case SCOPE_STATIC_CLASS:
			case SCOPE_STATIC_VAR:
				//TODO do this later
				error("Cannot get static function/variable/class from instance");
				exit(1);
			case SCOPE_LOCAL_FUNC:
				error("Wrapping object inside of function has not been implemented");
				exit(1);
			case SCOPE_LOCAL_CLASS:
				error("Wrapping object inside of function has not been implemented");
				exit(1);
			}
			assert(varType==SCOPE_LOCAL_VAR);
			if(lT->layoutType!=POINTER_LAYOUT && eval.getType()==R_LOC){
				unsigned int l =lT->getDataClassIndex(filePos,right);
				//TODO wrap into Lazy location
				Value* p = eval.getMyLocation()->getPointer(a,filePos);
				auto v = a.builder.CreateConstGEP2_32(p,0,l);
				return DATA::getLocation(new StandardLocation(v), returnType);
			}
			Value* lVal = eval->getValue(a,filePos);
			if(lVal!=NULL){
				if(lVal->getType()->isVectorTy()){
					return new ConstantData(a.builder.CreateExtractElement(lVal, getInt(lT->getDataClassIndex(filePos,right)),"getV"), returnType);
				} else if(lVal->getType()->isStructTy()){
					std::vector<unsigned int> b =  {lT->getDataClassIndex(filePos,right)};
					Value* t= a.builder.CreateExtractValue(lVal,ArrayRef<unsigned int>(b),"getV");
					return new ConstantData(t, returnType);
				} else if(lVal->getType()->isPointerTy()){
					Type* innerType = ((PointerType*) (lVal->getType()))->getElementType();
					if(innerType->isVectorTy() || innerType->isStructTy()){
						unsigned int ind = lT->getDataClassIndex(filePos,right);
						auto t = a.builder.CreateConstGEP2_32(lVal,0,ind);
						return new LocationData(new StandardLocation(t), returnType);
					} else {
						error("can't fast-lookup non-vector (2) ");
						return VOID;
					}
				} else {
					error("can't fast-lookup non-vector");
					return VOID;
				}
			} else {
				error("Could not find Value to get");
				return VOID;
			}*/
	}
};



#endif /* E_LOOKUP_HPP_ */
