/*
 * IfStatement.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef IFSTATEMENT_HPP_
#define IFSTATEMENT_HPP_

#include "./Statement.hpp"
#include "./Block.hpp"
#include "../containers/settings.hpp"

class IfStatement : public Statement{
	public:
		std::vector<std::pair<Statement*,Statement*>> condition;
		Statement* const finalElse;
		IfStatement(PositionID a, std::vector<std::pair<Statement*,Statement*>> & cond, Statement* const stat) :
			Statement(a, voidClass), condition(cond), finalElse(stat){
			if(condition.size()<1){
				cerr << "Cannot make if statement with no conditions";
				exit(0);
			}
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		const Token getToken() const override {
			return T_IF;
		}

		void registerClasses(RData& r) override final{
			for(auto& a: condition){
				a.first->registerClasses(r);
				a.second->registerClasses(r);
			}
			finalElse->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			for(auto& a: condition){
				a.first->registerFunctionArgs(r);
				a.second->registerFunctionArgs(r);
			}
			finalElse->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			for(auto& a: condition){
				a.first->registerFunctionDefaultArgs();
				a.second->registerFunctionDefaultArgs();
			}
			finalElse->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			for(auto& a: condition){
				a.first->resolvePointers();
				a.second->resolvePointers();
			}
			finalElse->resolvePointers();
		}
		ClassProto* checkTypes() override{
			for(auto& a:condition){
				if(a.first->checkTypes()!=boolClass) error("Cannot have non-bool as condition for if "+a.first->returnType->name);
				a.second->checkTypes();
			}
			finalElse->checkTypes();
			return returnType;
		}
		Value* evaluate(RData& r) override{
			error("If statement eval not implemented");
			/*
			for(auto &a: condition){
				if((bool) (a.first->evaluate())){
					a.second->evaluate(jump);
					return VOID;
				}
			}
			finalElse->evaluate(jump);
			return VOID;
			*/
		}
		Statement* simplify() override{
			std::vector<std::pair<Statement*,Statement*>> stack;
			for(auto &a: condition){
				Statement* sim = a.first->simplify();
				stack.push_back(std::pair<Statement*,Statement*>(sim,a.second->simplify()));
			}
			if(stack.size()==0){
				return finalElse->simplify();
			}
			else{
				return new IfStatement(filePos, stack,finalElse->simplify());
			}
		}
		void write(ostream& a,String t) const override{
			a << "if " << condition[0].first << " ";
			condition[0].second->write(a,t);
			a << endl;
			for(unsigned int i = 1; i<condition.size();++i){
				a << t << "else if(" << condition[i].first << ")";
				condition[i].second->write(a,t);
				if(finalElse->getToken()==T_VOID || i<condition.size()-1)
				a << endl;
			}
			if(finalElse->getToken()!=T_VOID){
				a << t << "else " << finalElse;
			}
		}
};


#endif /* IFSTATEMENT_HPP_ */
