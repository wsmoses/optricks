/*
 * ClassFunction.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef CLASSFUNCTION_HPP_
#define CLASSFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
class ClassFunction : public E_FUNCTION{
	public:
		Statement* body;
		Resolvable thisPointer;
		String name;
		AbstractClass* upperClass;
		VariableReference* returnV;
		ClassFunction(PositionID id, std::vector<Declaration*> dec, Resolvable& tPointer,String nam, VariableReference* a, VariableReference* b, Statement* r):
			E_FUNCTION(id, dec),thisPointer(tPointer){
			body = r;
			name =nam;
			prototype->name =self->getFullName()+"."+name;
			upperClass = nullptr;
			returnV = b;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			if(returnV) f << returnV->getFullName() << " ";
			f << (prototype->name);
			f << "(" ;
			bool first = true;
			for(auto &a: declaration){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			body->write(f, b+"  ");
		}
		void registerFunctionPrototype(RData& ra) override{
			if(myFunction) return;
			upperClass = self->getSelfClass();
			assert(upperClass);
			funcMap& funcs = upperClass->addLocalFunction(filePos,name);
			funcs.add(DATA::getFunction(NULL,prototype),filePos);
			std::vector<Type*> args;
			if(upperClass->layoutType==POINTER_LAYOUT || upperClass->layoutType==PRIMITIVEPOINTER_LAYOUT)
				args.push_back(upperClass->getType(ra));
			else args.push_back(upperClass->getType(ra)->getPointerTo());
			for(auto & b: prototype->declarations){
				b->registerFunctionPrototype(ra);
				AbstractClass* cla = b->checkTypes();
				Type* cl = cla->type;
				assert(cl!=NULL);
				args.push_back(cl);
			}
			if(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT) thisPointer.setObject(DATA::getConstant(NULL, upperClass));
			else thisPointer.setObject(DATA::getLocation(NULL, upperClass));
			const AbstractClass* cp = returnV->getSelfClass();
			if(cp==NULL) error("Unknown return type");
			if(cp==autoClass){
				std::vector<const AbstractClass*> yields;
				ret->collectReturns(yields,autoClass);
				cp = getMin(yields,filePos);
				if(cp==autoClass)  error("!Cannot support auto return for function");
				returnV = new ClassProtoWrapper(cp);
			} else if(returnV->getToken()!=T_CLASSPROTO) returnV = new ClassProtoWrapper(cp);
			prototype->returnType = cp;
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
			myFunction = ra.CreateFunction("!"+self->getFullName()+"."+name,FT, LOCAL_FUNC);
			funcs.set(DATA::getFunction(myFunction,prototype), filePos);
			ret->registerFunctionPrototype(ra);
		};
		void buildFunction(RData& ra) override final{
			if(built) return;
			ofunction::buildFunction(ra);
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = ra.CreateBlock1("entry", myFunction);
			//BasicBlock *MERGE = ra.CreateBlock1("funcMerge", myFunction);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = myFunction->arg_begin();
			AI->setName("this");
			if(upperClass->layoutType==POINTER_LAYOUT || upperClass->layoutType==PRIMITIVEPOINTER_LAYOUT)
				thisPointer.setObject(DATA::getConstant(AI, upperClass));
			else{
				assert(dyn_cast<PointerType>(AI->getType()));
				Type* t = ((PointerType*)(AI->getType()))->getElementType();
				Location* myLoc;
				if(t->isAggregateType() || t->isArrayTy() || t->isVectorTy() || t->isStructTy()){
					myLoc = new StandardLocation(AI);
				} else {
					myLoc = new LazyLocation(ra,AI,ra.builder.GetInsertBlock(),NULL);
				}
				thisPointer.setObject(DATA::getLocation(myLoc, upperClass));
			}
			//TODO create aggregatelocation
			AI++;
			for (; Idx+1 != myFunction->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer.name);
				prototype->declarations[Idx]->variable->getMetadata().setObject(DATA::getConstant(AI,prototype->declarations[Idx]->variable->returnType).toLocation(ra));
			}
			ra.guarenteedReturn = false;
			ret->checkTypes();
			ret->evaluate(ra);
			if( !ra.guarenteedReturn){
				if(prototype->returnType==voidClass)
					ra.builder.CreateRetVoid();
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			ra.FinalizeFunction(myFunction);
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
			ret->buildFunction(ra);
		};
};



#endif /* CLASSFUNCTION_HPP_ */
