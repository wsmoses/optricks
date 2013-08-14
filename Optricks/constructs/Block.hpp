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
		Block() :values(){

		}
		Block(std::vector<Statement*>& v) :values(v){

		}
		void push_back(Statement* s){
			return values.push_back(s);
		}
		Value* evaluate(RData& a,LLVMContext& c) override{
			todo("Block eval not implemented");
			/*
			for(auto& a:values){
				a->evaluate(jump);
				if(jump.type!=NJUMP){
					return VOID;
				}
			}
			return VOID;*/
		}
		Statement* simplify(Jump& jump) override{
			std::vector<Statement*> v;
			for(auto& a:values){
				Statement* s = a->simplify(jump);
				if(s->getToken()!=T_VOID){
					if(s->getToken()!=T_BLOCK){
						v.push_back(s);
					} else {
						Block* inner = (Block*)s;
						for(auto& b: inner->values){
							v.push_back(b);
						}
					}
				}
				if(jump.type!=NJUMP){
					break;
				}
			}
			return new Block(v);
		}
		void write(ostream& s,String start="") const override{
			s << "{" << endl;
			for(const auto& a:values){
				s << start << "  ";
				a->write(s,start+"  ");
				s << ";" << endl;
			}
			s << start << "}";
		}
		Token const getToken() const override{
			return T_BLOCK;
		}
};



#endif /* BLOCK_HPP_ */
