/*
 * Block.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include "Statement.hpp"

class Block : public Construct{
	public:
		std::vector<Statement*> values;
		void registerClasses(RData& r) override final{
			for(auto& a: values) a->registerClasses(r);
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
			for(auto& a:values) a->collectReturns(r, vals);
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
		Block(PositionID a) : Construct(a,voidClass),values(){}
		DATA evaluate(RData& r) override{
			bool ret = false;
			for(auto& a:values){
				if(ret) error("Already had guarenteed return");
				r.guarenteedReturn = false;
				a->evaluate(r);
				if(r.guarenteedReturn) ret = true;
			}
			r.guarenteedReturn = ret;
			return NULL;
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
		ClassProto* checkTypes(RData& r){
			for(auto& a:values){
				a->checkTypes(r);
			}
			return returnType;
		}
		Token const getToken() const override{
			return T_BLOCK;
		}
};



#endif /* BLOCK_HPP_ */
