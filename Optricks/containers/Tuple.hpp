/*
 * Tuple.hpp
 *
 *  Created on: Nov 2, 2013
 *      Author: wmoses
 */

#ifndef TUPLE_HPP_
#define TUPLE_HPP_
#include "ClassProto.hpp"

class NamedTupleClass:public ClassProto{
	private:
		static String condense(std::vector<std::pair<ClassProto*,String>>& t){
			String s="(";
			bool first = true;
			for(auto& a:t){
				if(first){
					first = false;
				} else s+=",";
				s+=a.first->name+":"+a.second;
			}
			return s+")";
		}
		NamedTupleClass(std::vector<std::pair<ClassProto*,String>>& t) : ClassProto(NULL,condense(t),NULL,PRIMITIVE_LAYOUT,false,true){
			for(auto& a:t){
				auto len = innerData.size();
				innerData.push_back(a.first);
				innerDataIndex[a.second]=len;
			}
		}
	public:
		char id() const override{ return 1; }
		static NamedTupleClass* get(std::vector<std::pair<ClassProto*,String>>& t){
			static std::vector<NamedTupleClass*> all_tuples;
			for(NamedTupleClass*& a: all_tuples){
				if(a->innerData.size()!=t.size()) continue;
				bool error = false;
				for(unsigned int i = 0; i<t.size(); i++){
					auto found = a->innerDataIndex.find(t[i].second);
					if(found==a->innerDataIndex.end() || found->second!=i || a->innerData[i]!=t[i].first){
						error = true;
						break;
					}
				}
				if(error) continue;
				else return a;
			}
			NamedTupleClass* tc = new NamedTupleClass(t);
			all_tuples.push_back(tc);
			return tc;
		}
		virtual bool hasCast(ClassProto* right) const override{
			if(right->id()!=id()) return false;
			if(this==right) return true;
			if(innerData.size()!=right->innerData.size()) return false;
			for(unsigned int i = 0; i<innerData.size(); i++){
				if(!innerData[i]->hasCast(right->innerData[i])) return false;
			}
			for(auto& a: innerDataIndex){
				auto f = right->innerDataIndex.find(a.first);
				if(f==right->innerDataIndex.end() || f->second!=a.second) return false;
			}
			return true;
		}
		virtual ouop* getCast(ClassProto* right, PositionID pid) override{
			if(right->id()!=id()) pid.error("Could not find cast from "+name+" to "+right->name);
			auto r = casts.find(right);
			if(r!=casts.end()) return r->second;
			if(innerData.size()!=right->innerData.size()) pid.error("Could not find cast from "+name+" to "+right->name);
			for(unsigned int i = 0; i<innerData.size(); i++){
				if(!innerData[i]->hasCast(right->innerData[i])) pid.error("Could not find cast from "+name+" to "+right->name);
			}
			for(auto& a: innerDataIndex){
				auto f = right->innerDataIndex.find(a.first);
				if(f==right->innerDataIndex.end() || f->second!=a.second) pid.error("Could not find cast from "+name+" to "+right->name);
			}
			return casts[right]=new ouopElementCast(this,right);
		}
};
class UnnamedTupleClass:public ClassProto{
	private:
		static String condense(std::vector<ClassProto*>& t){
			String s="(";
			bool first = true;
			for(auto& a:t){
				if(first){
					first = false;
				} else s+=",";
				s+=a->name;
			}
			return s+")";
		}
		UnnamedTupleClass(std::vector<ClassProto*>& t) : ClassProto(NULL,condense(t),NULL,PRIMITIVE_LAYOUT,false,true){
			for(auto& a:t){
				auto len = innerData.size();
				innerData.push_back(a);
				std::stringstream s;
				s<< '_';
				s<<len;
				innerDataIndex[s.str()]=len;
			}

//			constructors.add(DATA::getFunction(), )
		}
	public:
		char id() const override{ return 2; }
		static UnnamedTupleClass* get(std::vector<ClassProto*>& t){
			static std::vector<UnnamedTupleClass*> uall_tuples;
			for(UnnamedTupleClass*& a: uall_tuples){
				if(a->innerData.size()!=t.size()) continue;
				bool error = false;
				for(unsigned int i = 0; i<t.size(); i++){
					if(a->innerData[i]!=t[i]){
						error = true;
						break;
					}
				}
				if(error) continue;
				else return a;
			}
			UnnamedTupleClass* tc = new UnnamedTupleClass(t);
			uall_tuples.push_back(tc);
			return tc;
		}
		virtual bool hasCast(ClassProto* right) const override{
			if(!(right->id()==id() || right->id()==1)) return false;
			if(this==right) return true;
			if(innerData.size()!=right->innerData.size()) return false;
			for(unsigned int i = 0; i<innerData.size(); i++){
				if(!innerData[i]->hasCast(right->innerData[i])) return false;
			}
			return true;
		}
		virtual ouop* getCast(ClassProto* right, PositionID pid) override{
			if(!(right->id()==id() || right->id()==1)) pid.error("Could not find cast from "+name+" to "+right->name);
			auto r = casts.find(right);
			if(r!=casts.end()) return r->second;
			if(innerData.size()!=right->innerData.size()) pid.error("Could not find cast from "+name+" to "+right->name);
			for(unsigned int i = 0; i<innerData.size(); i++){
				if(!innerData[i]->hasCast(right->innerData[i])) pid.error("Could not find cast from "+name+" to "+right->name);
			}
			return casts[right]=new ouopElementCast(this,right);
		}
};

class MapClass:public ClassProto{

};


#endif /* TUPLE_HPP_ */
