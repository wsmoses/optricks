/*
 * ClassProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */


#include "settings.hpp"
#include "RData.hpp"
#include "operations.hpp"
#ifndef CLASSPROTO_HPP_
#define CLASSPROTO_HPP_

#define CLASSPROTO_C_
class ClassProto{
	private:
	std::map<String,std::map<ClassProto*, obinop*> > binops;
	std::map<ClassProto*, ouop*> casts;

		DATA (*constructor)(RData&, std::vector<Statement*>,PositionID,String);
		ClassProto* superClass;
		Type* type;
		std::map<String, unsigned int> innerDataIndex;
		std::vector<ClassProto*> innerData;
		std::map<String, ReferenceElement* > functions;
		//		std::map<String,std::map<ClassProto*, obinop*> > binops;
	public:
		String name;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		virtual ~ClassProto(){};
		virtual bool equals(ClassProto* c) const{
			return this==c;
		}
		virtual ClassProto* getSuper() const{
			return superClass;
		}
		/**
		 * Checks if this can be casted to C
		 */
		virtual std::pair<bool, unsigned int> compatable(ClassProto* c) const;
		unsigned int getDataClassIndex(String nam, PositionID id){
			if(innerDataIndex.find(nam)==innerDataIndex.end()) todo("Cannot find inner data type for class "+name+" named "+nam,id);
			return innerDataIndex[nam];
		}
		ClassProto* getDataClass(String nam, PositionID id){
			if(innerDataIndex.find(nam)==innerDataIndex.end()) todo("Cannot find inner data type for class "+name+" named "+nam,id);
			return innerData[innerDataIndex[nam]];
		}
		void addElement(String nam, ClassProto* typ,PositionID id){
			if(innerDataIndex.find(nam)!=innerDataIndex.end()) todo("Cannot create another inner data type for class "+name+" named "+nam,id);
			auto a = innerData.size();
			innerData.push_back(typ);
			innerDataIndex[nam]=a;
		}
		DATA construct(RData& r, std::vector<Statement*> s, PositionID i) const{
			return constructor(r,s,i,name);
		}
		Type* getType(RData& r){
			if(type!=NULL || innerData.size()==0) return type;
			else{
				std::vector<Type*> types;
				for(auto& a: innerData) types.push_back(a->getType(r));
				return StructType::create(ArrayRef<Type*>(types),name);
			}
		}
		virtual bool operator == (ClassProto*& b){
			return this == b;
		}
		bool hasCast(ClassProto* right){
			auto found = casts.find(right);
			return found!=casts.end();
		}
		Value* castTo(RData& r, DATA c, ClassProto* right){
			if(hasCast(right)) return casts[right]->apply(c, r);
			else{
				todo("Compile error - could not find cast "+name+" to "+right->name,PositionID());
				return NULL;
			}
		}
		ouop*& addCast(ClassProto* right){
			if(hasCast(right))
				todo("Error: Redefining cast "+name+" to "+right->name,PositionID());
			return casts[right];
		}
		obinop*& addBinop(String operation, ClassProto* right){
			auto found = binops.find(operation);
			if(found!=binops.end()){
				auto found2 = found->second.find(right);
				if(found2!=found->second.end()){
					todo("Error: Redefining binary operation '"+operation+"' from "+name+" to "+right->name,
							PositionID());
				}
			}
			return binops[operation][right];
		}
		std::pair<obinop*, std::pair<ouop*,ouop*> > getBinop(PositionID id, String operation, ClassProto* right){
			ClassProto* self = this;
			while(self!=NULL){
				auto found = self->binops.find(operation);
				if(found==self->binops.end()){
					self = self->superClass;
					continue;
				}
				auto thisToSelf = this->casts.find(self);
				if(thisToSelf==this->casts.end()){
					self = self->superClass;
					continue;
				}
				auto look = found->second;
				auto toCheck = right;
				while(toCheck!=NULL){
					auto found2 = look.find(toCheck);
					if(found2==look.end()){
						toCheck = toCheck->superClass;
						continue;
					}
					auto found3 = right->casts.find(toCheck);
					if(found3==right->casts.end()){
						toCheck = toCheck->superClass;
						continue;
					}
					return std::pair<obinop*,std::pair<ouop*,ouop*> >(found2->second,
							std::pair<ouop*,ouop*>(thisToSelf->second, found3->second)
					);
				}
				self = self->superClass;
			}
			todo("Binary operator "+operation+" not implemented for class "+
					name+ " with right "+ right->name, id);
			return std::pair<obinop*,std::pair<ouop*,ouop*> >(NULL,
					std::pair<ouop*,ouop*>(NULL,NULL));
		}
		ClassProto* leastCommonAncestor(ClassProto* c){
			std::set<ClassProto*> mySet;
			std::vector<ClassProto*> todo = {this, c};
			while(todo.size()>0){
				ClassProto* tmp = todo.back();
				auto tmp2 = mySet.find(tmp);
				if(tmp2!=mySet.end()) return *(tmp2);
				else{
					todo.pop_back();
					if(tmp->superClass!=NULL) todo.push_back(tmp->superClass);
					mySet.insert(tmp);
				}
			}
			return NULL;
		}
		ClassProto(ClassProto* sC, String n, DATA (*co)(RData&, std::vector<Statement*>,PositionID,String), Type* t=NULL) : constructor(co), superClass(sC), type(t),
				innerDataIndex((sC==NULL)?(std::map<String, unsigned int>()):(sC->innerDataIndex)),
				innerData((sC==NULL)?(std::vector<ClassProto*>()):(sC->innerData)),
				functions((sC==NULL)?(std::map<String, ReferenceElement* >()):(sC->functions)),
				name(n)
				 {
			casts.insert(std::pair<ClassProto*, ouop*>(this,new ouopNative([](Value* a, RData& m) -> Value*{	return a; }
								, this)));
		}
};

ClassProto* objectClass = new ClassProto(NULL, "object", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}));
ClassProto* autoClass = new ClassProto(NULL, "auto", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}));
ClassProto* classClass = new ClassProto(objectClass, "class", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}));
//ClassProto* autoClass = new ClassProto("auto"); todo auto class
//ClassProto* nullClass = new ClassProto("None");
ClassProto* boolClass = new ClassProto(objectClass, "bool", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}) ,BOOLTYPE);
ClassProto* functionClass = new ClassProto(objectClass, "function", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}));
ClassProto* complexClass = new ClassProto(objectClass,"complex", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	//todo("Cannot instantiate class "+s,id);
	//return NULL;
	double data[2] = {0, 0} ;
	return ConstantDataVector::get(r.lmod->getContext(), ArrayRef<double>(data));
			//	for(unsigned int i = 0; i<value.length(); ++i){
			//		data[0] = ConstantInt::get(CHARTYPE, value[0], false);
			//	}
}),COMPLEXTYPE);
ClassProto* doubleClass = new ClassProto(complexClass, "double", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}), DOUBLETYPE);
ClassProto* intClass = new ClassProto(doubleClass, "int", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}), INTTYPE);
ClassProto* stringClass = new ClassProto(objectClass, "string", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}),STRINGTYPE);
ClassProto* charClass = new ClassProto(stringClass, "char", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}), CHARTYPE);
ClassProto* sliceClass = new ClassProto(objectClass, "slice", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}));
ClassProto* voidClass = new ClassProto(objectClass, "void", ([](RData& r, std::vector<Statement*> vals,PositionID id,String s) -> DATA {
	todo("Cannot instantiate class "+s,id);
	return NULL;
}),VOIDTYPE);


String getGenericName(ClassProto* a, std::vector<ClassProto*>& b){
	String t = a->name+"<";
	bool first = true;
	for(auto& c:b){
		if(first) first = false;
		else t+=",";
		t+=c->name;
	}
	return t+">";
}
std::pair<bool, unsigned int> ClassProto::compatable(ClassProto* c) const{
			if(c==autoClass) return std::pair<bool, unsigned int>(true, UINT_MAX);
			const ClassProto* temp = this;
			int count = 0;
			while(temp!=NULL){
				if(temp->equals(c)) return std::pair<bool, unsigned int>(true,count);
				count++;
				temp = temp->getSuper();
			}
			return std::pair<bool, unsigned int>(false, 0);
		}
/*
class GeneralClass: public ClassProto{
	public:
		GeneralClass(ClassProto* t) : ClassProto(t,"",(Type*)NULL){

		}
};
/// *
class GenericClass: public ClassProto{
	public:
		GeneralClass* containerClass;
		//todo create type
		GenericClass(GeneralClass* T, std::vector<ClassProto*>& inner) : ClassProto(
				(T==NULL)?(T->superClass):NULL,
				getGenericName(T, inner),
				(Type*)NULL //TODO create generic struct type for arbitrary
		){
			containerClass = T;
		}

		bool operator == (ClassProto*& b) override final{
					GenericClass *bb;
					if((bb = dynamic_cast<GenericClass*>(b))==NULL) return (size_t)this==(size_t)b;
					return (size_t)this==(size_t)b;//TODO
				}
};
*/

#endif /* CLASSPROTO_HPP_ */
