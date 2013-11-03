/*
 * E_TUPLE.hpp
 *
 *  Created on: Nov 3, 2013
 *      Author: wmoses
 */

#ifndef E_TUPLE_HPP_
#define E_TUPLE_HPP_

#include "../constructs/Statement.hpp"
class E_TUPLE : public Statement{
	public:
		std::vector<Statement*> values;
		ClassProto* myClass;
		virtual ~E_TUPLE(){};
		E_TUPLE(PositionID id, const std::vector<Statement*>& a) :Statement(id,NULL),values(a),myClass(NULL) { };
		const  Token getToken() const override{
			return T_TUPLE;
		};

		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata of tuple");
		}
		String getFullName() override{
			if(myClass!=NULL) return myClass->name;
			String s="(";
			bool first = true;
			for(auto& a:values){
				if(first){
					first = false;
				} else s+=",";
				s+=a->getFullName();
			}
			return s+")";
		}
		void write(ostream& f,String a="") const override{
			f << "(";
			for(unsigned int i = 0; i<values.size(); ++i){
				f << values[i];
				if(i<values.size()-1 || i==0) f << ", ";
			}
			f << ")";
		}
		DATA evaluate(RData& m) override {
			if(returnType==NULL) checkTypes(m);
			if(returnType==classClass){
				if(myClass!=NULL) return DATA::getClass(myClass);
				std::vector<ClassProto*> cp(values.size());
				for(unsigned int i = 0; i<values.size(); i++){
					ClassProto* const tmp = values[i]->getSelfClass(m);
					if(tmp==NULL || tmp==voidClass || tmp==autoClass) values[i]->filePos.error("Cannot determine class of inner data for tuple");
					cp[i] = tmp;
				}
				return DATA::getClass(UnnamedTupleClass::get(cp));
			} else {
			Value* nextV = UndefValue::get(returnType->getType(m));
			for(unsigned int i = 0; i<values.size(); i++){
				ArrayRef<unsigned int> ar = ArrayRef<unsigned int>(std::vector<unsigned int>({i}));
				nextV = m.builder.CreateInsertValue(nextV,values[i]->evaluate(m).getValue(m),ar);
			}
			return DATA::getConstant(nextV,returnType);
			}
		}

		Statement* simplify() override{
			return this;
		}

		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		void registerClasses(RData& r) override final{
			for(auto& a:values){
				a->registerClasses(r);
			}
		}
		void registerFunctionPrototype(RData& r) override final{
			for(auto& a:values){
				a->registerFunctionPrototype(r);
			}
		};
		void buildFunction(RData& r) override final{
			for(auto& a:values){
				a->buildFunction(r);
			}
		};
		ClassProto* checkTypes(RData& r) override{
			std::vector<ClassProto*> cp(values.size());
			int isClass = 0;
			for(unsigned int i = 0; i<values.size(); i++){
				ClassProto* const tmp = values[i]->checkTypes(r);
				if(tmp==NULL || tmp==voidClass || tmp==autoClass) values[i]->filePos.error("Cannot determine class of inner data for tuple");
				if(tmp==classClass){
					if(isClass==0 || isClass==1) isClass = 1;
					else values[i]->filePos.error("Cannot have actual variable among tuple-class definition");
				} else isClass = 2;

				cp[i] = tmp;
			}
			if(isClass==1) return returnType=classClass;
			else return returnType = UnnamedTupleClass::get(cp);
		}
};


class E_NAMED_TUPLE : public Statement{
	public:
		std::vector<std::pair<Statement*,String>> values;
		ClassProto* myClass;
		virtual ~E_NAMED_TUPLE(){};
		E_NAMED_TUPLE(PositionID id, const std::vector<std::pair<Statement*,String>>& a) :Statement(id,classClass),values(a),myClass(NULL) { };
		const  Token getToken() const override{
			return T_NAMED_TUPLE;
		};

		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata of named_tuple");
		}
		String getFullName() override{
			if(myClass!=NULL) return myClass->name;
			String s="(";
			bool first = true;
			for(auto& a:values){
				if(first){
					first = false;
				} else s+=",";
				s+=a.first->getFullName()+":"+a.second;
			}
			return s+")";
		}
		void write(ostream& f,String a="") const override{
			f << "(";
			for(unsigned int i = 0; i<values.size(); ++i){
				f << values[i].first << ":" << values[i].second;
				if(i<values.size()-1 || i==0) f << ", ";
			}
			f << ")";
		}
		DATA evaluate(RData& m) override {
				if(myClass!=NULL) return DATA::getClass(myClass);
				std::vector<std::pair<ClassProto*,String> > cp(values.size());
				for(unsigned int i = 0; i<values.size(); i++){
					ClassProto* const tmp = values[i].first->getSelfClass(m);
					if(tmp==NULL || tmp==voidClass || tmp==autoClass) values[i].first->filePos.error("Cannot determine class of inner data for tuple");
					cp[i] = std::pair<ClassProto*,String>(tmp,values[i].second);
				}
				return DATA::getClass(NamedTupleClass::get(cp));
		}

		Statement* simplify() override{
			return this;
		}

		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		void registerClasses(RData& r) override final{
			for(auto& a:values){
				a.first->registerClasses(r);
			}
		}
		void registerFunctionPrototype(RData& r) override final{
			for(auto& a:values){
				a.first->registerFunctionPrototype(r);
			}
		};
		void buildFunction(RData& r) override final{
			for(auto& a:values){
				a.first->buildFunction(r);
			}
		};
		ClassProto* checkTypes(RData& r) override{
			for(unsigned int i = 0; i<values.size(); i++){
				ClassProto* const tmp = values[i].first->checkTypes(r);
				if(tmp==NULL || tmp==voidClass || tmp==autoClass) values[i].first->filePos.error("Cannot determine class of inner data for tuple");
				if(tmp!=classClass) values[i].first->filePos.error("Cannot have non-class in type declaration");
			}
			return returnType=classClass;
		}
};


#endif /* E_TUPLE_HPP_ */
