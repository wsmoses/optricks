/*
 * OClass.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "../language/class/UserClass.hpp"
#include "../language/ffi/F_Class.hpp"
#include "../language/ffi/F_Function.hpp"
#include "../language/ffi/F_Var.hpp"
class OClass: public ErrorStatement
{
	public:
		UserClass* proto;
		OClass* outer;
		std::vector<Statement* > under;
		std::vector<std::pair<bool,Declaration*> > data;/*true if static*/
		mutable bool buildF,eval,registerF,registerC;
		OClass(PositionID id, UserClass* p, OClass* out):ErrorStatement(id),
				proto(p), outer(out), under(), data(){
			buildF = eval = registerF = registerC = false;
		}
		/*String getFullName() override{
			if(outerClass==NULL) return proto->name;
			else return outerClass->getFullName()+proto->name;
		}*/
		const Token getToken() const override final {
			return T_CLASS;
		}


		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			id.error("Cannot use class-declaration as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			filePos.error("Cannot use class-declaration as data");
			exit(1);
		}
		const Data* evaluate(RData& r) const override final{
			if(proto==NULL) registerClasses();
			if(!eval){
				eval = true;
				for(Statement* const& a: under) a->evaluate(r);
				for(const auto& a: data) if(a.first) a.second->evaluate(r);
			}
			return proto;
		}
		void registerClasses() const override final{
			if(!registerC){
				registerC = true;
				if(outer){
					proto->addLocalVariable(filePos, "#outside", outer->proto);
				}
				for(const auto& a:under){
					a->registerClasses();
				}

				if(proto->layout==PRIMITIVEPOINTER_LAYOUT){
					if(data.size()>0) error("Cannot have data inside class with data layout of primitive_pointer");
				}
				else{
					for(const auto& d:data){
						if(!d.first) {
							proto->addLocalVariable(filePos,d.second->variable.pointer.name, d.second->getClass(filePos));
						}
					}
					//TODO allow default in constructor
				}
				proto->finalize(filePos);
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			if(!registerF){
				registerF = true;
				for(Statement* const& a:under) a->registerFunctionPrototype(r);
			}
		}
		void buildFunction(RData& r) const override final{
			if(!buildF){
				buildF = true;
				for(Statement* const& a:under) a->buildFunction(r);
			}
		}
};

void initClasses(){
	convertClass<int>::convert(&LANG_M);
	convertClass<long>::convert(&LANG_M);
	convertClass<long long>::convert(&LANG_M);
	//convertClass<size_t>::convert(&LANG_M);
	convertClass<int64_t>::convert(&LANG_M);
	//add_import_c_function(&LANG_M, malloc);
	add_import_c_function(&LANG_M, clock);
	add_import_c_function(&LANG_M, difftime);
	add_import_c_const(&LANG_M, CLOCKS_PER_SEC);

	add_import_c_function(&LANG_M, rand);
	add_import_c_function(&LANG_M, srand);
	add_import_c_const(&LANG_M, RAND_MAX);

	add_import_c_function(&LANG_M, qsort);
	add_import_cpp_function(&LANG_M, std::terminate);

	add_import_c_var(&LANG_M, errno, &NS_LANG_C.staticVariables);
	add_import_c_var(&LANG_M, stdout, &NS_LANG_C.staticVariables);
	//add_import_c_function(&LANG_M, mktime);

	//(&LANG_M)->addVariable(PositionID("#internal",0,0), "CLOCKS_PER_SEC", import_c_var_h<typeof(CLOCKS_PER_SEC)>::import(CLOCKS_PER_SEC));

	//add_import_c_function(&LANG_M, time);
	//add_import_c_function(&LANG_M, asctime);
	//add_import_c_function(&LANG_M, ctime);
	//add_import_c_function(&LANG_M, gmtime);
	//add_import_c_function(&LANG_M, localtime);
	//add_import_c_function(&LANG_M, strftime);

	//LANG_M.addScope()
	/*
	initClassesMeta();
	AbstractClass* cl[] = {classClass, objectClass, autoClass, boolClass,
			doubleClass, complexClass, intClass, charClass,
			sliceClass, voidClass,
			c_stringClass,c_intClass, c_longClass, c_long_longClass, c_pointerClass,byteClass};
	for(AbstractClass*& p:cl){
		//TODO fix this
		LANG_M->addClass(PositionID(0,0,"oclass#init"),p);
	}
	*/
}




#endif /* OCLASS_HPP_ */
