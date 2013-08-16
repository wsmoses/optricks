/*
 * ofunction.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OFUNCTION_HPP_
#define OFUNCTION_HPP_

#include "oobject.hpp"
#include "../constructs/Declaration.hpp"
class ofunction:public oobject{
	public:
		Resolvable* self;
		Resolvable* returnV;
		std::vector<Declaration*> declarations;
		ofunction(Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):oobject(functionClass),
			self(s),returnV(r),declarations(dec){
			self->resolveMeta() = this;
		}

		operator String () const override{
			std::stringstream f;
			f << "function " << returnV->name << " " << self->name << "(";
			bool first = true;
			for(auto &a: declarations){
				if(first) first = false;
				else f << ", ";
				a->write(f,"");
			}
			f << ")";
			return f.str();
		}
		void checkTypes(){
			for(auto& a:declarations){
				a->checkTypes();
			}
		}
};
//TODO
class externFunction : public ofunction{
	public:
		externFunction(Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):ofunction(s,r,dec){
		}
	Value* evaluate(RData& a,LLVMContext& context) override{
		//TODO implement
		todo("Extern not implemented");
	}
};
class nativeFunction : public ofunction{

};
class lambdaFunction : public ofunction{

};
class userFunction : public ofunction{

};

#endif /* OFUNCTION_HPP_ */
