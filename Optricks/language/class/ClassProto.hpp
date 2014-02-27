/*
 * ClassProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef CLASSPROTO_HPP_
#define CLASSPROTO_HPP_
/*
#include "settings.hpp"
#include "FunctionProto.hpp"
#include "Scopable.hpp"
#include "operations.hpp"


//WARNING from now on all classes which are eqv must have eqv pointers
//TODO update static/nonstatic member data
#define CLASSPROTO_C_
class ClassProto{
protected:
	const bool allowsInner;
//	std::map<String,std::map<ClassProto*, obinop*> > binops;
//	std::map<ClassProto*, ouop*> casts;
	mutable Type* type;
	//std::map<String, unsigned int> localDataIndex;
public:
//	std::vector<ClassProto*> localVariables;
//	funcMap constructors;
	const LayoutType layoutType;
//	const bool isGen;
	const String name;
//	std::map<String,ouop* > preops;
//	std::map<String,ouop* > postops;
	virtual ~ClassProto(){};
	virtual char id() const{ return 0; }
	/**
	 * Checks if this can be casted to C
	 */
/*
	virtual std::pair<bool, unsigned int> compatable(ClassProto* c) const;
	ClassProto* getDataClass(const unsigned int nam, PositionID id) const{
		if(allowsInner){
			std::vector<const ClassProto*> todo;
			const ClassProto* tmp = this;
			while(tmp!=NULL){
				todo.push_back(tmp);
				tmp = (ClassProto*) tmp->surroundingScope;
			}
			unsigned int ind=nam;
			while(todo.size()>0){
				tmp = todo.back();
				todo.pop_back();
				if(ind< tmp->localVariables.size()) return tmp->localVariables[nam];
				ind-=tmp->localVariables.size();
			}
		}
		id.error("Data index "+str<unsigned int>(nam)+" was too large for class "+name);
		exit(1);
	}
	unsigned int getDataClassIndex(PositionID id,String nam){
		if(allowsInner){
			std::vector<const ClassProto*> todo;
			const ClassProto* tmp = this;
			while(tmp!=NULL){
				todo.push_back(tmp);
				tmp = (ClassProto*) tmp->surroundingScope;
			}
			unsigned int ind=0;
			while(todo.size()>0){
				tmp = todo.back();
				todo.pop_back();
				auto a = tmp->mapping.find(nam);
				if(a!=tmp->mapping.end()) return ind+a->second.pos;
				ind+=tmp->localVariables.size();
			}
		}
		id.error("Inner data type "+nam+" could not be found in class "+name);
		return 0;
	}

	void addLocalVariable(PositionID id, String nam,ClassProto* d){
		if(!allowsInner) id.error("Class "+name+" does not allow inside elements to be created");
		if(exists(name)) id.error("Cannot define variable "+nam+" -- identifier already used in class "+name);
		mapping.insert(std::pair<String,SCOPE_POS>(name,SCOPE_POS(SCOPE_LOCAL_VAR,localVariables.size())));
		localVariables.push_back(d);
	}
	ClassProto*& getLocalVariable(PositionID id, String name){
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_LOCAL_VAR) id.error(name+" found at current scope, but not correct variable type -- needed class variable");
		return ((ClassProto*) f.first)->localVariables[f.second->second.pos];
	}
	ClassProto* getLocalReturnClass(PositionID id, String name);
	funcMap& getLocalFunction(PositionID id, String name){
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_LOCAL_FUNC) id.error(name+" found at current scope, but not correct variable type -- needed class method");
		return ((ClassProto*) f.first)->funcs[f.second->second.pos];
	}
	funcMap& addLocalFunction(PositionID id, String name){
		auto f = find2(id,name);
		if(f.first==NULL){
			mapping.insert(std::pair<String,SCOPE_POS>(name, SCOPE_POS(SCOPE_LOCAL_FUNC,funcs.size())));
			funcs.push_back(funcMap());
			return funcs.back();
		}
		else{
			if(f.second->second.type!=SCOPE_LOCAL_FUNC) id.error(name+" found at current scope, but not correct variable type -- needed class method");
			return ((ClassProto*) f.first)->funcs[f.second->second.pos];
		}
	}


	Value* generateData(RData& r);
	DATA construct(RData& r, E_FUNC_CALL* call) const;
	void addTypes(std::vector<Type*>& v,RData& r) const{
		if(surroundingScope!=NULL) ((ClassProto*)surroundingScope)->addTypes(v,r);
		for(ClassProto* const a: localVariables) v.push_back(a->getType(r));
	}
	Type* getType(RData& r);
	virtual bool operator == (ClassProto*& b){
		return this == b;
	}
	bool hasSuper(ClassProto* t) const{
		assert(t!=NULL);
		Scopable* c = this->surroundingScope;
		while(c!=NULL){
			if(c==t) return true;
			else c = c->surroundingScope;
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
				self = (ClassProto*) self->surroundingScope;
				continue;
			}
			//auto thisToSelf = this->casts.find(self);
			if(!hasCast(self)){
				self = (ClassProto*) self->surroundingScope;
				continue;
			}
			auto look = found->second;
			ClassProto* toCheck = right;
			while(toCheck!=NULL){
				auto found2 = look.find(toCheck);
				if(found2==look.end()){
					toCheck = (ClassProto*) toCheck->surroundingScope;
					continue;
				}
				//auto found3 = right->casts.find(toCheck);
				if(!right->hasCast(toCheck)){
					toCheck = (ClassProto*) toCheck->surroundingScope;
					continue;
				}
				return std::pair<obinop*,std::pair<ouop*,ouop*> >(found2->second,
						std::pair<ouop*,ouop*>(getCast(self,id), right->getCast(toCheck,id))
				);
			}
			self = (ClassProto*) self->surroundingScope;
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
			auto tmp = todo.back();
			auto tmp2 = mySet.find(tmp);
			if(tmp2!=mySet.end()) return *(tmp2);
			else{
				todo.pop_back();
				if(tmp->surroundingScope!=NULL) todo.push_back((ClassProto*) tmp->surroundingScope);
				mySet.insert(tmp);
			}
		}
		id.error("No common ancestor for "+name+" and "+c->name);
		exit(1);
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
	const Scopable* temp = this;
	int count = 0;
	while(temp!=NULL){
		if(temp==c) return std::pair<bool, unsigned int>(true,count);
		count++;
		temp = temp->surroundingScope;
	}
	auto f = casts.find(c);
	if(f!=casts.end()) return std::pair<bool,unsigned int>(true,UINT_MAX-1);
	return std::pair<bool, unsigned int>(false, 0);
}*/
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
/*
#include "operations.hpp"

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
		Constant* newInside[from->localVariables.size()];
		for(unsigned int i = 0; i<from->localVariables.size(); i++){
			Value* tv = DATA::getConstant(s->getAggregateElement(i), from->localVariables[i]).castToV(m, to->localVariables[i],id);
			if(Constant* c = dyn_cast<Constant>(tv))
				newInside[i] = c;
			else{
				id.error("Constant cast ran into a problem -- a necessary inner cast did not produce a constant.");
			}
		}
		return DATA::getConstant(ConstantStruct::get(dyn_cast<StructType>(to->getType(m)),ArrayRef<Constant*>( newInside,from->localVariables.size() )),to);
	}
	Value* nextV = UndefValue::get(to->getType(m));
	for(unsigned int i = 0; i<from->localVariables.size(); i++){
		ArrayRef<unsigned int> ar = ArrayRef<unsigned int>(i);
		Value* iv = DATA::getConstant(m.builder.CreateExtractValue(v,ar), from->localVariables[i]).castToV(m, to->localVariables[i],id);
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


ClassProto* ClassProto::getLocalReturnClass(PositionID id, String name){
	auto f = find(id,name);
	switch(f.second->second.type){
	case SCOPE_LOCAL_VAR:
		return ((ClassProto*) f.first)->localVariables[f.second->second.pos];
	case SCOPE_LOCAL_FUNC:
		return functionClass;
	case SCOPE_LOCAL_CLASS:
		return classClass;
	default:
		id.error(name+" found at current scope, but was not local -- needed local variable/class");
		exit(1);
	}
}*/
/*
ClassProto* ReferenceElement::getReturnType(RData& r) const{
	if(funcs.size()>0) return functionClass;
	else return llvmObject.getReturnType(r);
}*/
#endif /* CLASSPROTO_HPP_ */
