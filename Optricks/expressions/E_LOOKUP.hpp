/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../constructs/Statement.hpp"
class E_LOOKUP : public Statement{
	public:
		const Token getToken() const override{ return T_LOOKUP; }
		Statement* left;
		String right;
		virtual ~E_LOOKUP(){};
		E_LOOKUP(PositionID id, Statement* a,  String b): Statement(id),
				left(a), right(b){};
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{
		}
		void write(ostream& f,String a="") const override{
			f << left << "." << right;
		}

		String getFullName() override final{
			return left->getFullName()+"."+right;
		}
		void registerClasses(RData& r) override final{
			left->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			left->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) override final{
			left->buildFunction(r);
		};
		ClassProto* checkTypes(RData& r){
			if(returnType!=NULL) return returnType;
			ClassProto* superC = left->checkTypes(r);
			if(superC==classClass){
				ClassProto* c = left->getSelfClass(r);
				if(c->hasClass(right)) return returnType = classClass;
				else error("Could not look up/check other static initializers "+right+" in "+c->name);
			}
			if(superC->hasFunction(right)) return returnType = functionClass;
			return returnType = superC->getDataClass(right,filePos);
		}
		DATA evaluate(RData& a) override{
			checkTypes(a);
			DATA eval = left->evaluate(a);
			///STATIC STUFF
			if(eval.getType()==R_CLASS){
				ClassProto* c = eval.getMyClass(a);
				if(c->hasClass(right)){
					return c->getClass(right, filePos)->getObject(filePos);
				} 	else error("Could not look up other static initializers "+right);
			}
			/*write(cerr);
			cerr << endl << flush;
			cerr << eval.getReturnType(a) << endl << flush;
			if(eval.getType()==R_LOC || eval.getType()==R_CONST){
				((Value*)(eval.getPointer()))->dump();
			}
			cerr << eval.getType() << endl << flush;*/
			ClassProto* lT = eval.getReturnType(a);
			if(lT->hasFunction(right)){
				//TODO add wrapper around object which called function
				return lT->getFunction(right, filePos)->getObject(filePos);
			} else if(lT->layoutType!=POINTER_LAYOUT && eval.getType()==R_LOC){
				unsigned int l =lT->getDataClassIndex(right,filePos);
				Value* v = a.builder.CreateConstGEP2_32(eval.getMyLocation(),0,l);
				return DATA::getLocation(v, returnType);
			}
			Value* lVal = eval.getValue(a);
			if(lVal!=NULL){
				if(lVal->getType()->isVectorTy()){
					return DATA::getConstant(a.builder.CreateExtractElement(lVal, getInt(lT->getDataClassIndex(right,filePos)),"getV"), returnType);
				} else if(lVal->getType()->isStructTy()){
					std::vector<unsigned int> b =  {lT->getDataClassIndex(right,filePos)};
					Value* t= a.builder.CreateExtractValue(lVal,ArrayRef<unsigned int>(b),"getV");
					return DATA::getConstant(t, returnType);
				} else if(lVal->getType()->isPointerTy()){
					Type* innerType = ((PointerType*) (lVal->getType()))->getElementType();
					if(innerType->isVectorTy() || innerType->isStructTy()){
						unsigned int ind = lT->getDataClassIndex(right,filePos);
						Value* t = a.builder.CreateConstGEP2_32(lVal,0,ind);
						return DATA::getLocation(t, returnType);
					} else {
						error("can't fast-lookup non-vector (2) ");
						return DATA::getNull();
					}
				} else {
					error("can't fast-lookup non-vector");
					return DATA::getNull();
				}
			} else {
				error("Could not find Value to get");
				return DATA::getNull();
			}
		}
		Statement* simplify() override{
			return this;
		}
		ReferenceElement* getMetadata(RData& r){
			checkTypes(r);
			auto lT = left->checkTypes(r);

			if(lT->hasFunction(right)) return lT->getFunction(right, filePos);
			else return new ReferenceElement("", NULL,lT->name+"."+right, evaluate(r), funcMap());
		}
};



#endif /* E_LOOKUP_HPP_ */
