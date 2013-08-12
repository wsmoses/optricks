/*
 * indexed_map.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef INDEXED_MAP_HPP_
#define INDEXED_MAP_HPP_

#include <unordered_map>
#include <map>
//TODO allow unordered map
template <typename KEY, typename VALUE>
class indexed_map{
		std::map<KEY, unsigned int> mapping;
		std::vector<VALUE> data;
		VALUE & operator [] (KEY & ind){
			bool found = data.find(ind) == data.end();
			if(found) return data[mapping[ind]];
			else{
				data[ind] = data.size();
				data.push_back(VALUE());
				return data[data.size()-1];
			}
		}
		int getIndex(KEY& ind){
			bool found = data.find(ind) == data.end();
			if(found) return data[ind].first;
			else return -1;
		}
};

#endif /* INDEXED_MAP_HPP_ */
