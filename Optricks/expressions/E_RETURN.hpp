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

		String getFullName() override final{
			error("Cannot get full name of return");
			return "";
		}
		Constant* getConstant(RData& r) override final { return NULL; }
		ClassProto* getSelfClass() override final{
			error("Cannot get selfClass of return"); return NULL;
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_RETURN");
			return inner->getMetadata(r);
		}

		void registerClasses(RData& r) override final{
			if(inner!=NULL)
			inner->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			if(inner!=NULL)
			inner->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			if(inner!=NULL)
			inner->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			if(inner!=NULL)
			inner->resolvePointers();
		}
		Statement* simplify() override{
			return this;
		}
		DATA evaluate(RData& r) override {
			Value* t = NULL;
	//		BasicBlock *Parent = r.builder.GetInsertBlock();
	//		BasicBlock *RETB = BasicBlock::Create(getGlobalContext(), "ret");
	//		r.builder.SetInsertPoint(RETB);
			if(inner!=NULL && inner->getToken()!=T_VOID){
				t = inner->evaluate(r);
				if(t==NULL) error("Why is t null?");
			}
			auto toBreak = r.getBlock(name, jump, (inner==NULL)?voidClass:(inner->returnType), r.builder.GetInsertBlock(), t, r);
	//		auto toBreak = r.getBlock(name, jump, returnType, RETB, t);

			r.builder.CreateBr(toBreak);
			//if(jump==RETURN)
				r.guarenteedReturn = true;
			return NULL;
	//		return RETB;
			//if(returnType==voidClass) return r.builder.CreateRetVoid();
			//else return r.builder.CreateRet(t);
		}
		void write (ostream& f,String b="") const override{
			switch(jump){
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
