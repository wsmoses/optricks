/*
 * ClassTemplate.hpp
 *
 *  Created on: May 28, 2014
 *      Author: Billy
 */

#ifndef CLASSTEMPLATE_HPP_
#define CLASSTEMPLATE_HPP_
#include "../includes.hpp"

enum TemplateArgType{
	TEMPLATE_CLASS,
	TEMPLATE_INT
};
struct TemplateArg{
	TemplateArgType argType;
	union TemplateValue{
		const AbstractClass* t_class;
		mpz_t t_int;
	} argValue;
	TemplateArg(const AbstractClass* a){
		argType = TEMPLATE_CLASS;
		argValue.t_class = a;
	}
	TemplateArg(const mpz_t& v){
		argType = TEMPLATE_INT;
		mpz_init_set(argValue.t_int,v);
	}
};
class ClassTemplate{
public:
	virtual ~ClassTemplate(){};
	virtual const AbstractClass* getClass(const std::vector<TemplateArg>&) const;
};

class SingleClassTemplate : public ClassTemplate{
private:
	const AbstractClass* tr;
public:
	SingleClassTemplate(const AbstractClass* ac) : tr(){

	}
	const AbstractClass* getClass(const std::vector<TemplateArg>&) const{
		return tr;
	}
};

class BuiltinClassTemplate : public ClassTemplate{
private:
	const std::function<const AbstractClass*(const std::vector<TemplateArg>&)> inlined;
public:
	BuiltinClassTemplate(const std::function<const AbstractClass*(const std::vector<TemplateArg>&)>& ac) : inlined(ac){
	}
	const AbstractClass* getClass(const std::vector<TemplateArg>& a) const{
		return inlined(a);
	}
};

#endif /* CLASSTEMPLATE_HPP_ */
