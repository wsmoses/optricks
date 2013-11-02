/*
 * E_RETURN.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_RETURN_HPP_
#define E_RETURN_HPP_
#include "../constructs/Statement.hpp"
class E_RETURN : public Statement{
	public:
		virtual ~E_RETURN(){};
		Statement* inner;
		String name;
		JumpType jump;
		E_RETURN(PositionID id, Statement* t, String n, JumpType j) :
			Statement(id, voidClass), inner(t), name(n), jump(j){ };
		const Token getToken() const override{
			return T_RETURN;
		};
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe){
			ClassProto* n = (inner==NULL)?voidClass:(inner->checkTypes(r));
			if(toBe==autoClass) vals.push_back(n);
			else if(!n->hasCast(toBe)) error("Could not cast type "+n->name +" to "+toBe->name);
		}
		String getFullName() override final{
			error("Cannot get full name of return");
			return "";
		}
		ClassProto* getSelfClass(RData& r) override final{
			error("Cannot get selfClass of return"); return NULL;
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_RETURN");
			return NULL;
		}
		void registerClasses(RData& r) override final{
			if(inner!=NULL)
			inner->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			if(inner!=NULL) inner->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			if(inner!=NULL) inner->buildFunction(r);
		}
		Statement* simplify() override{
			return this;
		}
		DATA evaluate(RData& r) override {
			DATA t = DATA::getNull();
			if(inner!=NULL && inner->getToken()!=T_VOID){
				t = inner->evaluate(r);
			}
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock *RESUME = BasicBlock::Create(getGlobalContext(), "postReturn", TheFunction);

			BasicBlock* toBreak = r.getBlock(name, jump, r.builder.GetInsertBlock(), t, filePos, std::pair<BasicBlock*,BasicBlock*>(r.builder.GetInsertBlock(),RESUME));

			if(toBreak!=NULL) r.builder.CreateBr(toBreak);
			//if(jump==RETURN)
			if(jump==YIELD){
				r.builder.SetInsertPoint(RESUME);
			} else {
				RESUME->removeFromParent();
				r.guarenteedReturn = true;
			}
			return DATA::getNull();
	//		return RETB;
			//if(returnType==voidClass) return r.builder.CreateRetVoid();
			//else return r.builder.CreateRet(t);
		}
		void write (ostream& f,String b="") const override{
			switch(jump){
				case YIELD:
					f  << "yield";
					if(inner!= NULL && inner->getToken()!=T_VOID) f << " " << inner;
					return;
				case RETURN:
					f  << "return";
					if(inner!= NULL && inner->getToken()!=T_VOID) f << " " << inner;
					return;
				case BREAK:
					f  << "break";
					if(name!= "" ) f << " " << name;
					return;
				case CONTINUE:
					f  << "continue";
					if(name!= "" ) f << " " << name;
					return;
			}
		}
		ClassProto* checkTypes(RData& r) override final{
			if(inner!=NULL) inner->checkTypes(r);
			return NULL;
		}
};


#endif /* E_RETURN_HPP_ */
