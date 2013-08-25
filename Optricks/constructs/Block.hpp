/*
 * Block.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include "Statement.hpp"

class Block : public Statement{
	public:
		std::vector<Statement*> values;
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); return NULL; }
		void registerClasses(RData& r) override final{
			for(auto& a: values) a->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			for(auto& a: values) a->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			for(auto& a: values) a->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			for(auto& a: values) a->resolvePointers();
		}
		Block(PositionID a) : Statement(a,voidClass),values(){

		}
		Value* evaluate(RData& r) override{
			for(auto& a:values) a->evaluate(r);
			return NULL;
				//TODO check;
//			error("Block eval not implemented");
			/*
			for(auto& a:values){
				a->evaluate(jump);
				if(jump.type!=NJUMP){
					return VOID;
				}
			}
			return VOID;*/
		}
		Statement* simplify() override{
			Block* b = new Block(filePos);
			std::vector<Statement*> v;
			for(auto& a:values){
				Statement* s = a->simplify();
				if(s->getToken()!=T_VOID) b->values.push_back(s);
			}
			if(b->values.size()==0){
				free(b);
				return VOID;
			}
			return b;
		}
		void write(ostream& s,String start="") const override{
			s << "{" << endl;
			for(const auto& a:values){
				if(a->getToken()==T_VOID) continue;
				s << start << "  ";
				a->write(s,start+"  ");
				s << ";" << endl;
			}
			s << start << "}";
		}
		ClassProto* checkTypes(){
			for(auto& a:values){
				a->checkTypes();
			}
			return returnType;
		}
		Token const getToken() const override{
			return T_BLOCK;
		}
};



#endif /* BLOCK_HPP_ */
