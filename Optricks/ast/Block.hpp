/*
 * Block.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include "../language/statement/Statement.hpp"

class Block : public ErrorStatement{
	public:
		OModule module;
		std::vector<Statement*> values;
		void registerClasses() const override final{
			for(auto& a: values) a->registerClasses();
		}
		void reset() const override final{
			for(auto& a: values) a->reset();
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe){
			for(auto& a:values) a->collectReturns(vals, toBe);
		}
		void registerFunctionPrototype(RData& r) const override final{
			for(auto& a: values) a->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			for(auto& a: values) a->buildFunction(r);
		}
		Block(PositionID a, OModule* supMod) : ErrorStatement(a),module(supMod),values(){}
		const Data* evaluate(RData& r) const override{
			for(auto& a:values){
				if(r.hadBreak()) error("Already had guaranteed return");
				a->evaluate(r);
			}
			if(!r.hadBreak())
				for(const auto & dat: module.vars){
					decrementCount(r, filePos, dat);
				}
			return &VOID_DATA;
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			id.error("Block cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const{
			return &voidClass;
		}
		Token const getToken() const override{
			return T_BLOCK;
		}
		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
};



#endif /* BLOCK_HPP_ */
