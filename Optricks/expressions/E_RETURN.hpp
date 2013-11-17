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
			DATA t = (inner!=NULL && inner->getToken()!=T_VOID)?(inner->evaluate(r)):(DATA::getNull());
			if(jump==YIELD){
				BasicBlock *RESUME = r.CreateBlock("postReturn");
				//BasicBlock* toBreak =
				r.getBlock(name, YIELD, r.builder.GetInsertBlock(), t, filePos, std::pair<BasicBlock*,BasicBlock*>(r.builder.GetInsertBlock(),RESUME));
				//assert(toBreak!=NULL);
//				r.addPred(toBreak,r.builder.GetInsertBlock());
				r.builder.SetInsertPoint(RESUME);
			} else if(jump==RETURN){
				if(
						//t.getReturnType(ar, filePos)==voidClass
						//TODO check this
						t.getType()==R_UNDEF)
					r.builder.CreateRetVoid();
				else r.builder.CreateRet(t.getValue(r,filePos));
				r.guarenteedReturn = true;
			} else {
				BasicBlock* toBreak = r.getBlock(name, jump, r.builder.GetInsertBlock(), t, filePos, std::pair<BasicBlock*,BasicBlock*>(r.builder.GetInsertBlock(),NULL));
				if(toBreak!=NULL) r.builder.CreateBr(toBreak);
				r.guarenteedReturn = true;
			}
			return DATA::getNull();
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
