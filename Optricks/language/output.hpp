/*
 * output.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef OUTPUT_HPP_
#define OUTPUT_HPP_
#include "includes.hpp"

template<> inline String str<String>(String s){
	return s;
}

template<> inline String str<char>(char c){
	return String(1,c);
}

template <class T> ostream& operator<<(ostream&os, std::vector<T>& v){
		bool first = true;
		os<<"[";
		for(const auto& a:v){
			if(first){
				first= false;
				os<<a;
			}else os<<", "<<a;
		}
		return os<<"]";
}


#endif /* OUTPUT_HPP_ */
