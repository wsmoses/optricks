/*
 * ClassProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */


#include "settings.hpp"
#include "FunctionProto.hpp"
#include "operations.hpp"
#ifndef CLASSPROTO_HPP_
#define CLASSPROTO_HPP_
//WARNING from now on all classes which are eqv must have eqv pointers
//TODO update static/nonstatic member data
#define CLASSPROTO_C_
class ClassProto{
		friend class RData;
		friend class DATA;
		friend class ouop;
		friend class obinop;
		friend class ouopElementCast;
		friend class UnnamedTupleClass;
		friend class NamedTupleClass;
	protected:
		const bool allowsInner;
		std::map<String,std::map<ClassProto*, obinop*> > binops;
		std::map<ClassProto*, ouop*> casts;
		mutable Type* type;
		std::map<String, unsigned int> innerDataIndex;
		std::vector<ClassProto*> innerData;
		std::map<String, ReferenceElement* > functions;
		std::map<String, ReferenceElement* > staticClasses;
		ClassProto* superClass;
		//TODO allow overloading / polymorphism
		bool hasInner(String s) const{
			const ClassProto* tmp = this;
			while(tmp!=NULL){
				if(tmp->innerDataIndex.find(s)!=tmp->innerDataIndex.end()) return true;
				if(tmp->functions.find(s)!=tmp->functions.end()) return true;
				if(tmp->staticClasses.find(s)!=tmp->staticClasses.end()) return true;
				tmp = tmp->superClass;
			}
			return false;
		}
	public:
		funcMap constructors;
		const LayoutType layoutType;
		const bool isGen;
		const String name;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		virtual ~ClassProto(){};
		virtual char id() const{ return 0; }
		//bool isPointerType() const{
		//	return layoutType==POINTER_LAYOUT || layoutType==PRIMITIVEPOINTER_LAYOUT;
		//}
		virtual ClassProto* getSuper() const{
			return superClass;
		}
		/**
		 * Checks if this can be casted to C
		 */
		virtual std::pair<bool, unsigned int> compatable(ClassProto* c) const;

		ClassProto* getDataClass(const unsigned int nam, PositionID id) const{
			if(allowsInner){
				std::vector<const ClassProto*> todo;
				const ClassProto* tmp = this;
				while(tmp!=NULL){
					todo.push_back(tmp);
					tmp = tmp->superClass;
				}
				unsigned int ind=nam;
				while(todo.size()>0){
					tmp = todo.back();
					todo.pop_back();
					if(ind< tmp->innerData.size()) return tmp->innerData[nam];
					ind-=tmp->innerData.size();
				}
			}
			id.error("Data index "+str<unsigned int>(nam)+" was too large for class "+name);
			return NULL;
		}
		unsigned int getDataClassIndex(String nam, PositionID id) const{
			if(allowsInner){
				std::vector<const ClassProto*> todo;
				const ClassProto* tmp = this;
				while(tmp!=NULL){
					todo.push_back(tmp);
					tmp = tmp->superClass;
				}
				unsigned int ind=0;
				while(todo.size()>0){
					tmp = todo.back();
					todo.pop_back();
					auto a = tmp->innerDataIndex.find(nam);
					if(a!=tmp->innerDataIndex.end()) return ind+a->second;
					ind+=tmp->innerData.size();
				}
			}
			id.error("Inner data type "+nam+" could not be found in class "+name);
			return 0;
		}
		ClassProto* getDataClass(String nam, PositionID id){
			if(allowsInner){
				ClassProto* tmp = this;
				while(tmp!=NULL){
					auto a = innerDataIndex.find(nam);
					if(a!=innerDataIndex.end()) return innerData[a->second];
					tmp = tmp->superClass;
				}
			}
			id.error("Inner data type "+nam+" could not be found in class "+name);
			return NULL;
		}

		void addClass(String nam,ReferenceElement* typ,PositionID id){
			if(nam==name) id.error("Cannot make data type with same name as class "+name);
			if(hasInner(nam)) id.error("Cannot create another inner data type for class "+name+" named "+nam);
			staticClasses[nam] = typ;
		}
		const bool hasClass(String name) const{
			return staticClasses.find(name)!=staticClasses.end();
		}

		ReferenceElement* getClass(String nam, PositionID id) const{
			auto a = staticClasses.find(name);
			if(a==staticClasses.end()) id.error("Inner class "+nam+" does not exist in class "+name);
			return a->second;
		}
		const bool hasFunction(String name) const{
			return functions.find(name)!=functions.end();
		}
		ReferenceElement* getFunction(String nam, PositionID id){
			if(!hasFunction(nam)) id.error("Function "+nam+" does not exist in class "+name);
			return functions[nam];
		}
		ReferenceElement* addFunction(String nam, PositionID id);
		void addElement(String nam, ClassProto* typ,PositionID id){
			if(nam==name) id.error("Cannot make data type with same name as class "+name);
			if(hasInner(nam)) id.error("Cannot create another inner data type for class "+name+" named "+nam);
			auto a = innerData.size();
			innerData.push_back(typ);
			innerDataIndex[nam]=a;
		}
		Value* generateData(RData& r);
		DATA construct(RData& r, E_FUNC_CALL* call) const;
		void addTypes(std::vector<Type*>& v,RData& r) const{
			if(superClass!=NULL) superClass->addTypes(v,r);
			for(ClassProto* const a: innerData) v.push_back(a->getType(r));
		}
		Type* getType(RData& r);
		virtual bool operator == (ClassProto*& b){
			return this == b;
		}
		bool hasSuper(ClassProto* t) const{
			assert(t!=NULL);
			const ClassProto* c = this->superClass;
			while(c!=NULL){
				if(c==t) return true;
				else c = c->superClass;
			}
			return false;
		}
		ouop*& addCast(ClassProto* right, PositionID id=PositionID(0,0,"<start.addCast>")){
			if(casts.find(right)!=casts.end())
				id.error("Error: Redefining cast "+name+" to "+right->name);
			return casts[right];
		}
		obinop*& addBinop(String operation, ClassProto* right, PositionID id=PositionID(0,0,"<start.addBinop>")){
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
				//auto thisToSelf = this->casts.find(self);
				if(!hasCast(self)){
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
					//auto found3 = right->casts.find(toCheck);
					if(!right->hasCast(toCheck)){
						toCheck = toCheck->superClass;
						continue;
					}
					return std::pair<obinop*,std::pair<ouop*,ouop*> >(found2->second,
							std::pair<ouop*,ouop*>(getCast(self,id), right->getCast(toCheck,id))
					);
				}
				self = self->superClass;
			}
			id.error("Binary operator "+operation+" not implemented for class "+
					name+ " with right "+ right->name);
			return std::pair<obinop*,std::pair<ouop*,ouop*> >(NULL,
					std::pair<ouop*,ouop*>(NULL,NULL));
		}
		virtual bool hasCast(ClassProto* right) const{
			if(this==right) return true;
			if(((layoutType==POINTER_LAYOUT && right->layoutType==POINTER_LAYOUT) || (layoutType==PRIMITIVEPOINTER_LAYOUT && right->layoutType==PRIMITIVEPOINTER_LAYOUT)) &&
					hasSuper(right)){
				return true;
			}
			return casts.find(right)!=casts.end();
		}
		virtual ouop* getCast(ClassProto* right, PositionID id){
			auto found = casts.find(right);
			if(found==casts.end())
				id.error("Compile error - could not find cast from "+name+" to "+right->name);
			return found->second;
		}
		ClassProto* leastCommonAncestor(ClassProto* c, PositionID id){
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
			id.error("No common ancestor for "+name+" and "+c->name);
			return NULL;
		}
		ClassProto(ClassProto* sC, String n, Type* t,LayoutType pointer,bool isGe,bool allowsInne=true);
};


class NullClass:public ClassProto{
	private:
		NullClass() : ClassProto(NULL,"`NullClass",C_POINTERTYPE,POINTER_LAYOUT,false,true){
		}
	public:
		char id() const override{ return 3; }
		static NullClass* get(){
			static NullClass* t=new NullClass();
			return t;
		}
		virtual bool hasCast(ClassProto* right) const override{
			return right->layoutType==POINTER_LAYOUT || right->layoutType==PRIMITIVEPOINTER_LAYOUT;
		}
		virtual ouop* getCast(ClassProto* right, PositionID pid) override{
			if(!(right->layoutType==POINTER_LAYOUT || right->layoutType==PRIMITIVEPOINTER_LAYOUT)) pid.error("Could not find cast from NullClass to "+right->name);
			auto r = casts.find(right);
			if(r!=casts.end()) return r->second;
			return casts[right]=new ouopNullCast(right);
		}
};

NullClass* nullClass = NullClass::get();
ClassProto* uint32Class = new ClassProto(NULL, "uint32",UINT32TYPE,PRIMITIVE_LAYOUT,false,false);

ClassProto* objectClass = new ClassProto(NULL, "object",NULL,POINTER_LAYOUT,false);
ClassProto* autoClass = new ClassProto(NULL, "auto",NULL,PRIMITIVE_LAYOUT,false);
ClassProto* classClass = new ClassProto(objectClass, "class",NULL,POINTER_LAYOUT,false);
ClassProto* boolClass = new ClassProto(NULL, "bool",BOOLTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* c_pointerClass = new ClassProto(NULL,"c_pointer",C_POINTERTYPE,PRIMITIVEPOINTER_LAYOUT,false,false);
ClassProto* functionClass = new ClassProto(NULL,"function",FUNCTIONTYPE,PRIMITIVEPOINTER_LAYOUT,false);
ClassProto* complexClass = new ClassProto(NULL,"complex",COMPLEXTYPE,PRIMITIVE_LAYOUT,false);
ClassProto* doubleClass = new ClassProto(NULL, "double",DOUBLETYPE,PRIMITIVE_LAYOUT,false,false);
//ClassProto* rationalClass = new ClassProto(doubleClass, "rational", RATIONALTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* intClass = new ClassProto(doubleClass, "int", INTTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* c_intClass = new ClassProto(NULL,"c_int",C_LONGTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* c_longClass = new ClassProto(NULL,"c_long",C_INTTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* c_long_longClass = new ClassProto(NULL,"c_long_long",C_LONG_LONGTYPE,PRIMITIVE_LAYOUT,false,false);
//ClassProto* stringClass = new ClassProto(objectClass,"string",NULL,POINTER_LAYOUT);
ClassProto* c_stringClass = new ClassProto(NULL, "c_string",C_STRINGTYPE,PRIMITIVEPOINTER_LAYOUT,false,false);
ClassProto* charClass = new ClassProto(c_stringClass, "char", CHARTYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* byteClass = new ClassProto(NULL, "byte", BYTETYPE,PRIMITIVE_LAYOUT,false,false);
ClassProto* sliceClass = new ClassProto(objectClass, "slice",SLICETYPE,PRIMITIVE_LAYOUT,false);
ClassProto* voidClass = new ClassProto(NULL, "void",VOIDTYPE,PRIMITIVE_LAYOUT,false,false);


DATA DATA::getClass(ClassProto* c){
	return DATA(R_CLASS, c, classClass);
};

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
		if(temp==c) return std::pair<bool, unsigned int>(true,count);
		count++;
		temp = temp->getSuper();
	}
	auto f = casts.find(c);
	if(f!=casts.end()) return std::pair<bool,unsigned int>(true,UINT_MAX-1);
	return std::pair<bool, unsigned int>(false, 0);
}
/*
class GenericClass: public ClassProto{
	public:
		ClassProto* containerClass;
		//todo create type
		GenericClass(ClassProto* T, std::vector<ClassProto*>& inner) : ClassProto(
				(T==NULL)?(T->superClass):NULL,
				getGenericName(T, inner),
				(Type*)NULL //TODO create generic struct type for arbitrary
		){
			containerClass = T;
		}

		bool operator == (ClassProto*& b) override final{
					GenericClass *bb;
					if((bb = d ynamic_cast<GenericClass*>(b))==NULL) return (size_t)this==(size_t)b;
					return (size_t)this==(size_t)b;//TODO
				}
};
 */

#include "operations.hpp"
DATA DATA::castTo(RData& r, ClassProto* right, PositionID id) const{
#ifdef NDEBUG
	if(!(type==R_CONST || type==R_LOC || type==R_FUNC)){
		cerr << "Compiler Error: Illegal data type cast " << type << endl << flush;
		exit(1);
	}
#else
	assert(type==R_CONST || type==R_LOC || type==R_FUNC);
#endif
	assert(right!=NULL);
	ClassProto* left = getReturnType(r);
	if(left == right) return *this;
	if(((left->layoutType==POINTER_LAYOUT && right->layoutType==POINTER_LAYOUT) || (left->layoutType==PRIMITIVEPOINTER_LAYOUT && right->layoutType==PRIMITIVEPOINTER_LAYOUT)) &&
			left->hasSuper(right)){
		if(type==R_LOC) return DATA::getLocation(data.location, right);
		else if(type==R_CONST) return DATA::getConstant(data.constant, right);
		else assert(0 && "this type is invalid");
	}
	ouop* c = left->getCast(right,id);
	return c->apply(*this, r, id);
}

ClassProto* getMin(std::vector<ClassProto*>& vals, PositionID id){
	if(vals.size()==0) return voidClass;
	ClassProto* tmp = vals[0];
	for(unsigned int i =1 ; i<vals.size(); i++){
		if(tmp==NULL || tmp==autoClass) id.error("Could not determine minimum type.");
		tmp = tmp->leastCommonAncestor(vals[i], id);
	}
	return tmp;
}

DATA ouopElementCast::apply(DATA a, RData& m, PositionID id){
	Value* v = a.getValue(m,id);
	if(ConstantStruct* s = dyn_cast<ConstantStruct>(v)){
		Constant* newInside[from->innerData.size()];
		for(unsigned int i = 0; i<from->innerData.size(); i++){
			Value* tv = DATA::getConstant(s->getAggregateElement(i), from->innerData[i]).castToV(m, to->innerData[i],id);
			if(Constant* c = dyn_cast<Constant>(tv))
				newInside[i] = c;
			else{
				id.error("Constant cast ran into a problem -- a necessary inner cast did not produce a constant.");
			}
		}
		return DATA::getConstant(ConstantStruct::get(dyn_cast<StructType>(to->getType(m)),ArrayRef<Constant*>( newInside,from->innerData.size() )),to);
	}
	Value* nextV = UndefValue::get(to->getType(m));
	for(unsigned int i = 0; i<from->innerData.size(); i++){
		ArrayRef<unsigned int> ar = ArrayRef<unsigned int>(i);
		Value* iv = DATA::getConstant(m.builder.CreateExtractValue(v,ar), from->innerData[i]).castToV(m, to->innerData[i],id);
		nextV = m.builder.CreateInsertValue(nextV,iv,ar);
	}
	return DATA::getConstant(nextV,to);
}

obinopNative* NULLCHECK1 = new obinopNative([](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
	Value* val =av.getValue(m,id);
	Value* temp = m.builder.CreateICmpEQ(val,ConstantPointerNull::get((PointerType*) val->getType()),"cmptmp");
	return DATA::getConstant(temp,boolClass);
},boolClass);
obinopNative* NULLCHECK2 = new obinopNative([](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
	Value* val =av.getValue(m,id);
	Value* temp = m.builder.CreateICmpNE(val,ConstantPointerNull::get((PointerType*) val->getType()),"cmptmp");
	return DATA::getConstant(temp,boolClass);
},boolClass);
obinopNative* NULLCHECK3 = new obinopNative([](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
	Value* val =bv.getValue(m,id);
	Value* temp = m.builder.CreateICmpEQ(val,ConstantPointerNull::get((PointerType*) val->getType()),"cmptmp");
	return DATA::getConstant(temp,boolClass);
},boolClass);
obinopNative* NULLCHECK4 = new obinopNative([](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
	Value* val =bv.getValue(m,id);
	Value* temp = m.builder.CreateICmpNE(val,ConstantPointerNull::get((PointerType*) val->getType()),"cmptmp");
	return DATA::getConstant(temp,boolClass);
},boolClass);
#endif /* CLASSPROTO_HPP_ */
