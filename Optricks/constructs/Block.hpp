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
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe){
			for(auto& a:values) a->collectReturns(r, vals, toBe);
		}
		void registerFunctionPrototype(RData& r) override final{
			for(auto& a: values) a->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			for(auto& a: values) a->buildFunction(r);
		}
		Block(PositionID a) : Construct(a,voidClass),values(){}
		DATA evaluate(RData& r) override{
			bool ret = false;
			for(auto& a:values){
				if(ret) error("Already had guaranteed return");
				r.guarenteedReturn = false;
				a->evaluate(r);
				if(r.guarenteedReturn) ret = true;
			}
			r.guarenteedReturn = ret;
			return DATA::getNull();
		}
		Statement* simplify() override{
			unsigned int toPut = 0;
			for(unsigned int i = 0; i<values.size(); ++i){
				Statement* s = values[i]->simplify();
				if(s->getToken()!=T_VOID) values[toPut++] = s;
			}
			while(values.size()>toPut) values.pop_back();
			return this;
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
//				a->write(cerr); cerr << endl << flush;
				a->checkTypes(r);
			}
			return returnType;
		}
		Token const getToken() const override{
			return T_BLOCK;
		}
};



#endif /* BLOCK_HPP_ */
