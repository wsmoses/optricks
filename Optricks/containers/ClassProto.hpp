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
		Type* type;
		std::map<String, unsigned int> innerDataIndex;
		std::vector<ClassProto*> innerData;
		std::map<String, ReferenceElement* > functions;
		//		std::map<String,std::map<ClassProto*, obinop*> > binops;
		ClassProto* superClass;
	public:
		funcMap constructors;
		bool isPointer;
		String name;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		classFunction* iterator;
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
		const bool hasFunction(String name) const{
			return functions.find(name)!=functions.end();
		}
		ReferenceElement* getFunction(String nam){
			if(!hasFunction(nam)) return NULL;
			return functions[nam];
		}
		ReferenceElement* addFunction(String nam, PositionID id);
		void addElement(String nam, ClassProto* typ,PositionID id){
			if(nam==name) todo("Cannot make data type with same name as class "+name,id);
			if(innerDataIndex.find(nam)!=innerDataIndex.end()) todo("Cannot create another inner data type for class "+name+" named "+nam,id);
			if(functions.find(nam)!=functions.end()) todo("Cannot create another inner data type for class "+name+" named "+nam,id);
			auto a = innerData.size();
			innerData.push_back(typ);
			innerDataIndex[nam]=a;
		}
		DATA generateData(RData& r){
//			Type* mt = getType(r);
			return UndefValue::get(getType(r));
			/*
			if(mt->isVectorTy()){
				VectorType* vt = dynamic_cast<VectorType*>(mt);
				Type* t = vt->getElementType();
				auto count = vt->getNumElements();
				std::vector<Constant*> vals;
				for(unsigned int i = 0; i<count; i++)
					vals.push_back(UndefValue::get(t));
				return ConstantVector::get(ArrayRef<Constant*>(vals));
			} else if(mt->isArrayTy()){
				ArrayType* vt = dynamic_cast<ArrayType*>(mt);
				Type* t = vt->getElementType();
				auto count = vt->getNumElements();
				std::vector<Constant*> vals;
				for(unsigned int i = 0; i<count; i++)
					vals.push_back(UndefValue::get(t));
				return ConstantArray::get(vt, ArrayRef<Constant*>(vals));
			} else if(mt->isStructTy()){
				StructType* vt = dynamic_cast<StructType*>(mt);
				auto count = vt->getNumElements();
				std::vector<Constant*> vals;
				for(unsigned int i = 0; i<count; i++)
					vals.push_back(UndefValue::get(vt->getElementType(i)));
				return ConstantStruct::get(vt, ArrayRef<Constant*>(vals));
			}*/
		}
		DATA construct(RData& r, E_FUNC_CALL* call) const;
		Type* getType(RData& r){
			if(type!=NULL || innerData.size()==0) return type;
			else{
				std::vector<Type*> types;
				//TODO allow recursive type
				//cerr << "hmm" << endl << flush;
				for(auto& a: innerData) types.push_back(a->getType(r));
				//cerr << "end hmm" << endl << flush;
				type = StructType::create(r.lmod->getContext(), ArrayRef<Type*>(types),name);
				if(isPointer) type = type->getPointerTo(0);
				return type;
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
		ouop*& addCast(ClassProto* right, PositionID id=PositionID()){
			if(hasCast(right))
				id.error("Error: Redefining cast "+name+" to "+right->name);
			return casts[right];
		}
		obinop*& addBinop(String operation, ClassProto* right, PositionID id=PositionID()){
			auto found = binops.find(operation);
			if(found!=binops.end()){
				auto found2 = found->second.find(right);
				if(found2!=found->second.end()){
					id.error("Error: Redefining binary operation '"+operation+"' from "+name+" to "+right->name);
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
		ClassProto(ClassProto* sC, String n, Type* t=NULL,bool pointer=false) : isPointer(pointer), constructors(), superClass(sC), type(t),
				innerDataIndex((sC==NULL)?(std::map<String, unsigned int>()):(sC->innerDataIndex)),
				innerData((sC==NULL)?(std::vector<ClassProto*>()):(sC->innerData)),
				functions((sC==NULL)?(std::map<String, ReferenceElement* >()):(sC->functions)),
				name(n),iterator(NULL)
				 {
			casts.insert(std::pair<ClassProto*, ouop*>(this,new ouopNative([](Value* a, RData& m) -> Value*{	return a; }
								, this)));
		}
};

ClassProto* objectClass = new ClassProto(NULL, "object");
ClassProto* autoClass = new ClassProto(NULL, "auto");
ClassProto* classClass = new ClassProto(objectClass, "class");
//ClassProto* autoClass = new ClassProto("auto"); todo auto class
//ClassProto* nullClass = new ClassProto("None");
ClassProto* boolClass = new ClassProto(objectClass, "bool",BOOLTYPE);
ClassProto* functionClass = new ClassProto(objectClass, "function");
ClassProto* complexClass = new ClassProto(objectClass,"complex",COMPLEXTYPE);
ClassProto* doubleClass = new ClassProto(complexClass, "double",DOUBLETYPE);
ClassProto* intClass = new ClassProto(doubleClass, "int", INTTYPE);
ClassProto* c_intClass = new ClassProto(intClass,"c_int",C_LONGTYPE);
ClassProto* c_longClass = new ClassProto(intClass,"c_long",C_INTTYPE);
ClassProto* c_long_longClass = new ClassProto(intClass,"c_long_long",C_LONG_LONGTYPE);
ClassProto* stringClass = new ClassProto(objectClass,"string");
ClassProto* c_stringClass = new ClassProto(stringClass, "c_string",C_STRINGTYPE,true);
ClassProto* charClass = new ClassProto(c_stringClass, "char", CHARTYPE);
ClassProto* sliceClass = new ClassProto(objectClass, "slice");
ClassProto* voidClass = new ClassProto(objectClass, "void",VOIDTYPE);


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

ClassProto* getMin(std::vector<ClassProto*>& vals, PositionID id){
	if(vals.size()==0) return voidClass;
	ClassProto* tmp = vals[0];
	for(unsigned int i =1 ; i<vals.size(); i++){
		if(tmp==NULL || tmp==autoClass) id.error("Could not determine minimum type.");
		tmp = tmp->leastCommonAncestor(vals[i]);
	}
	return tmp;
}
#endif /* CLASSPROTO_HPP_ */
