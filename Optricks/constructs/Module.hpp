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

#define dataType Value*

class OModule : public Stackable{
	public:
		OModule* super;
		std::map<String, dataType> mapping;
		OModule(OModule* before){
			super = before;
		}
		const Token getToken() const override{
			return T_MODULE;
		}
		const int exists(String index) const{
			const OModule* search = this;
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
		dataType& operator[] (String index) {
			const OModule* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return mapping[index];
				}
			}
			return mapping[index];
		}
		void write(ostream& a,String t) const override{
			a << "Module";
		}
};

#undef dataType
OModule* LANG_M = new OModule(NULL);

RData::RData():
			module(new OModule(LANG_M)),
			builder(IRBuilder<>(getGlobalContext()))
			{
		}
#endif /* MODULE_HPP_ */
