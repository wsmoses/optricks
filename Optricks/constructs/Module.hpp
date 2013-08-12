/*
 * Module.hpp
 * Contains list of local variables for resolution
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Stackable.hpp"
#include "../containers/settings.hpp"
#include "../primitives/oobject.hpp"

class Module;

#define dataType Stackable*
class opointer{
	public:
		Module* module;
		unsigned int index;
		String name;
		opointer(){

		}
		opointer(Module* a, unsigned int b, String c) : module(a), index(b), name(c){
		}
		dataType resolve() const;
};

class Module : public Stackable{
	public:
		Module* super;
		std::vector<dataType> data;
		std::map<String, opointer> mapping;
		Module(Module* before){
			super = before;
		}
		const Token getToken() const override{
			return T_MODULE;
		}
		const int exists(String index) const{
			const Module* search = this;
			int level = 0;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
					level++;
				} else {
					return level;
				}
			}
			return -1;
		}
		opointer& addPointer(String index, dataType value, unsigned int level=0){
			if(level == 0){
				opointer nex(this, data.size(), index);
				data.push_back(value);
				mapping.insert(std::pair<String,opointer>(index, nex));
				return (mapping[index]);
			} else {
				if(super==NULL){
					cerr << "Null module to add pointer to" << endl << flush;
					exit(1);
				}
				else
				return super->addPointer(index, value, level-1);
			}
		}
		opointer& getPointer(String index) {
			const Module* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return mapping[index];
				}
			}
			cerr << "Could not resolve variable: " << index << flush << endl;
			exit(0);
		}
		void write(ostream& a,String t) const override{
			a << "Module";
		}
};

dataType opointer::resolve() const {
	return module->data[index];
}

#undef dataType
Module* LANG_M = new Module(NULL);
#endif /* MODULE_HPP_ */
