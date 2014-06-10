/*
 * OClass.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "../language/class/UserClass.hpp"
#include "../language/class/builtin/NullClass.hpp"
#include "../language/ffi/F_Class.hpp"
#include "../language/ffi/F_Function.hpp"
#include "../language/ffi/F_Var.hpp"
class OClass: public ErrorStatement
{
	public:
		UserClass* proto;
		OClass* outer;
		std::vector<Statement* > under;
		std::vector<std::pair<bool,Declaration*> > data;/*true if static*/
		mutable bool buildF,eval,registerF,registerC;
		OClass(PositionID id, UserClass* p, OClass* out):ErrorStatement(id),
				proto(p), outer(out), under(), data(){
			buildF = eval = registerF = registerC = false;
		}
		/*String getFullName() override{
			if(outerClass==NULL) return proto->name;
			else return outerClass->getFullName()+proto->name;
		}*/
		const Token getToken() const override final {
			return T_CLASS;
		}


		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			id.error("Cannot use class-declaration as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			filePos.error("Cannot use class-declaration as data");
			exit(1);
		}
		const Data* evaluate(RData& r) const override final{
			if(proto==NULL) registerClasses();
			if(!eval){
				eval = true;
				for(Statement* const& a: under) a->evaluate(r);
				for(const auto& a: data) if(a.first) a.second->evaluate(r);
			}
			return proto;
		}
		void registerClasses() const override final{
			if(!registerC){
				registerC = true;
				if(outer){
					proto->addLocalVariable(filePos, "#outside", outer->proto);
				}
				for(const auto& a:under){
					a->registerClasses();
				}

				if(proto->layout==PRIMITIVEPOINTER_LAYOUT){
					if(data.size()>0) error("Cannot have data inside class with data layout of primitive_pointer");
				}
				else{
					for(const auto& d:data){
						if(!d.first) {
							proto->addLocalVariable(filePos,d.second->variable.pointer.name, d.second->getClass(getRData(), filePos));
						}
					}
					//TODO allow default in constructor
				}
				proto->finalize(filePos);
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			if(!registerF){
				registerF = true;
				for(Statement* const& a:under) a->registerFunctionPrototype(r);
			}
		}
		void buildFunction(RData& r) const override final{
			if(!buildF){
				buildF = true;
				for(Statement* const& a:under) a->buildFunction(r);
			}
		}
};

void initClasses(){
	ComplexClass::get(&doubleClass);
	ComplexClass::get(&intLiteralClass);
	ComplexClass::get(&floatLiteralClass);
	convertClass(int,&LANG_M);
	convertClass(short,&LANG_M);
	convertClass(long,&LANG_M);
	convertClass(long long,&LANG_M);
	//convertClass(size_t,&LANG_M);
	convertClass(int64_t,&LANG_M);
	//add_import_c_function(&LANG_M, malloc);
	add_import_c_function(&LANG_M, clock);
	add_import_c_function(&LANG_M, difftime);
	add_import_c_const(&LANG_M, CLOCKS_PER_SEC);

	add_import_c_function(&LANG_M, rand);
	add_import_c_function(&LANG_M, srand);
	add_import_c_const(&LANG_M, RAND_MAX);

	add_import_c_function(&LANG_M, qsort);
	//add_import_cpp_function(&LANG_M, std::terminate);
	LANG_M.addFunction(PositionID(0,0,"#class"),"print")->add(
		new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(&classClass)},&voidClass),
		[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		const AbstractClass* ac = args[0]->evaluate(r)->getMyClass(r, id);
		auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
		std::stringstream s;
		s << "class{" << ac << ", '"<< ac->getName() << "'}";
		for(auto& a: s.str()){
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
		}
		return &VOID_DATA;}), PositionID(0,0,"#int"));
	LANG_M.addFunction(PositionID(0,0,"#class"),"println")->add(
		new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(&classClass)},&voidClass),
		[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		const AbstractClass* ac = args[0]->evaluate(r)->getMyClass(r, id);
		auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
		std::stringstream s;
		s << "class{" << ac << ", '"<< ac->getName() << "'}" << endl;
		for(auto& a: s.str()){
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
		}
		return &VOID_DATA;}), PositionID(0,0,"#int"));

	LANG_M.addClass(PositionID(0,0,"#complex"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
		if(args.size()==0) return ComplexClass::get(&doubleClass);
		if(args.size()!=1){
			id.error("Cannot use template class 'complex' with more than one argument");
			return nullptr;
		}
		switch(args[0]->classType){
		case CLASS_INT:
		case CLASS_FLOAT:
		case CLASS_INTLITERAL:
		case CLASS_FLOATLITERAL:
			return ComplexClass::get((RealClass*) args[0]);
		default:
			id.error("Cannot use class '"+args[0]->getName()+"' as template arg for class 'complex'");
		}
		return nullptr;
	}),"complex");
	LANG_M.addClass(PositionID(0,0,"#array"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
			//if(args.size()==0) return ComplexClass::get(&doubleClass);
			if(args.size()!=1){
				id.error("Must use template class 'array' with one argument");
				return nullptr;
			}
			return ArrayClass::get(args[0], 0);
		}),"array");
	LANG_M.addClass(PositionID(0,0,"#function"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
			if(args.size()==0) {
				id.error("Must use template class 'function' with at least one argument - found "+str(args.size()));
				return nullptr;
			}
			std::vector<const AbstractClass*> ar;
			for(unsigned i=1; i<args.size(); i++)
				ar.push_back(args[i]);
			return FunctionClass::get(args[0], ar);
		}),"function");

	LANG_M.addFunction(PositionID(0,0,"#str"),"scanf")->add(
			new BuiltinInlineFunction(
					new FunctionProto("scanf",{AbstractDeclaration(&stringLiteralClass)},&intClass,true),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()>=1);
			//TODO custom formatting for printf (and checks for literals / correct format / etc)
			const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
			llvm::SmallVector<llvm::Type*,1> t_args(1);
			t_args[0] = C_STRINGTYPE;
			auto CU = r.getExtern("scanf", llvm::FunctionType::get(c_intClass.type, t_args,true));
			llvm::SmallVector<llvm::Value*,1> m_args(args.size());
			m_args[0] = r.getConstantCString(value);
			for(unsigned i=1; i<args.size(); i++){
				const Data* D = args[i]->evaluate(r);
				if(D->getReturnType()->classType!=CLASS_REF){
					id.error("Cannot have non-reference as argument to 'scanf'");
					return &VOID_DATA;
				}
				assert(D->type==R_REF);
				m_args[i] = ((ReferenceData*)D)->value->getMyLocation()->getPointer(r, id);
			}
			llvm::Value* V = r.builder.CreateCall(CU, m_args);
			V = r.builder.CreateSExtOrTrunc(V, intClass.type);
			return new ConstantData(V, &intClass);
		}), PositionID(0,0,"#int"));

	auto SDL = new ScopeClass(&LANG_M, PositionID("#init",0,0), "sdl");
	{
		auto AF = new UserClass(&SDL->staticVariables,"AudioFormat",nullptr,PRIMITIVE_LAYOUT,false);
		SDL->staticVariables.addClass(PositionID("#sdl",0,0),AF);
		AF->addLocalVariable(PositionID("#sdl",0,0),"_data",&shortClass);
		AF->finalize(PositionID("#sdl",0,0));
		AF->constructors.add(new BuiltinInlineFunction(new FunctionProto("AudioFormat",{AbstractDeclaration(&shortClass)},AF),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* A = args[0]->evalV(r, id);
			llvm::Value* V = llvm::UndefValue::get(AF->type);
			return new ConstantData(r.builder.CreateInsertValue(V, A, 0), AF);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isFloat")->add(new BuiltinInlineFunction(new FunctionProto("isFloat",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_DATATYPE)),BOOLTYPE),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isBigEndian")->add(new BuiltinInlineFunction(new FunctionProto("isBigEndian",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_ENDIAN)),BOOLTYPE),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isSigned")->add(new BuiltinInlineFunction(new FunctionProto("isSigned",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_SIGNED)),BOOLTYPE),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isInt")->add(new BuiltinInlineFunction(new FunctionProto("isInt",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_DATATYPE)),BOOLTYPE)),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isLittleEndian")->add(new BuiltinInlineFunction(new FunctionProto("isLittleEndian",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_ENDIAN)),BOOLTYPE)),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isUnsigned")->add(new BuiltinInlineFunction(new FunctionProto("isUnsigned",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data", instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_SIGNED)),BOOLTYPE)),&boolClass);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("bitSize")->add(new BuiltinInlineFunction(new FunctionProto("bitSize",{AbstractDeclaration(AF)},&byteClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			llvm::Value* V = AF->getLocalData(r, id, "_data",instance)->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateAnd(V, llvm::ConstantInt::get(shortClass.type,SDL_AUDIO_MASK_BITSIZE)),byteClass.type),&byteClass);
		}), PositionID("#sdl",0,0));
#define SDL_A(A,B) AF->staticVariables.addVariable(PositionID("#sdl",0,0),#B, new ConstantData(getRData().builder.CreateInsertValue(llvm::UndefValue::get(AF->type), llvm::ConstantInt::get(shortClass.type,A,false), 0),AF));
		SDL_A(AUDIO_S8, S8);
		SDL_A(AUDIO_U8, U8);
		SDL_A(AUDIO_S16LSB, S16LSB);
		SDL_A(AUDIO_S16MSB, S16MSB);
		SDL_A(AUDIO_S16SYS, S16SYS);
		SDL_A(AUDIO_S16, S16);
		SDL_A(AUDIO_U16LSB, U16LSB);
		SDL_A(AUDIO_U16MSB, U16MSB);
		SDL_A(AUDIO_U16SYS, U16SYS);
		SDL_A(AUDIO_U16, U16);
		SDL_A(AUDIO_S32LSB, S32LSB);
		SDL_A(AUDIO_S32MSB, S32MSB);
		SDL_A(AUDIO_S32SYS, S32SYS);
		SDL_A(AUDIO_S32, S32);
		SDL_A(AUDIO_F32LSB, F32LSB);
		SDL_A(AUDIO_F32MSB, F32MSB);
		SDL_A(AUDIO_F32SYS, F32SYS);
		SDL_A(AUDIO_F32, F32);
#undef SDL_A


		auto AS = new UserClass(&SDL->staticVariables,"AudioSpec",nullptr,PRIMITIVE_LAYOUT,false);
		SDL->staticVariables.addClass(PositionID("#sdl",0,0),AS);
		AS->addLocalVariable(PositionID("#sdl",0,0),"freq",&c_intClass);
		AS->addLocalVariable(PositionID("#sdl",0,0),"format",AF);
		AS->addLocalVariable(PositionID("#sdl",0,0),"channels",&byteClass);
		AS->addLocalVariable(PositionID("#sdl",0,0),"silence",&byteClass);
		AS->addLocalVariable(PositionID("#sdl",0,0),"samples",&shortClass);
		AS->addLocalVariable(PositionID("#sdl",0,0),"size",&intClass);
		std::vector<const AbstractClass*> v= {&c_pointerClass,&c_pointerClass,&c_intClass};
		AS->addLocalVariable(PositionID("#sdl",0,0),"callback",FunctionClass::get(&voidClass,v));//
		AS->addLocalVariable(PositionID("#sdl",0,0),"userdata",&c_pointerClass);
		AS->finalize(PositionID("#sdl",0,0));
		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"init")->add(
				new CompiledFunction(new FunctionProto("init",{AbstractDeclaration(&intClass)},&c_intClass),
						getRData().getExtern("SDL_Init",&c_intClass,{&intClass})),PositionID("#sdl",0,0));
		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"initSubsystem")->add(
				new CompiledFunction(new FunctionProto("initSubSystem",{AbstractDeclaration(&intClass)},&c_intClass),
						getRData().getExtern("SDL_InitSubSystem",&c_intClass,{&intClass})),PositionID("#sdl",0,0));
		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"quit")->add(
						new CompiledFunction(new FunctionProto("quit",std::vector<AbstractDeclaration>({}),&voidClass),
								getRData().getExtern("SDL_Quit",&voidClass,{})),PositionID("#sdl",0,0));
		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"quitSubsystem")->add(
						new CompiledFunction(new FunctionProto("quitSubsystem",{AbstractDeclaration(&intClass)},&voidClass),
								getRData().getExtern("SDL_QuitSubSystem",&voidClass,{&intClass})),PositionID("#sdl",0,0));
#define LOAD(M,N)\
		SDL->staticVariables.addVariable(PositionID("#sdl",0,0),#N, new ConstantData(getInt32(M), &intClass));
		LOAD(SDL_INIT_TIMER,INIT_TIMER);
		LOAD(SDL_INIT_AUDIO,INIT_AUDIO);
		LOAD(SDL_INIT_VIDEO,INIT_VIDEO);
		LOAD(SDL_INIT_JOYSTICK,INIT_JOYSTICK);
		LOAD(SDL_INIT_HAPTIC,INIT_HAPTIC);
		LOAD(SDL_INIT_GAMECONTROLLER,INIT_GAMECONTROLLER);
		LOAD(SDL_INIT_EVENTS,INIT_EVENTS);
		LOAD(SDL_INIT_EVERYTHING,INIT_EVERYTHING);
		LOAD(SDL_INIT_NOPARACHUTE,INIT_NOPARACHUTE);
#undef LOAD
		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"getError")->add(
						new CompiledFunction(new FunctionProto("getError",&c_stringClass),
								getRData().getExtern("SDL_GetError",&c_stringClass,{})),PositionID("#sdl",0,0));

		SDL->staticVariables.addFunction(PositionID("#sdl",0,0),"clearError")->add(
						new CompiledFunction(new FunctionProto("clearError",&voidClass),
								getRData().getExtern("SDL_ClearError",&voidClass,{})),PositionID("#sdl",0,0));

	}
	{
		auto MUS = new UserClass(&SDL->staticVariables,"MusicType",nullptr,PRIMITIVE_LAYOUT,false);
				SDL->staticVariables.addClass(PositionID("#sdl",0,0),MUS);
				const AbstractClass* T;
				if(sizeof(Mix_MusicType)==sizeof(int))
					T=&c_intClass;
				else if(sizeof(Mix_MusicType)==sizeof(long))
					T=&c_longClass;
				else if(sizeof(Mix_MusicType)==sizeof(long long))
					T=&c_longlongClass;
				else{
					PositionID("#sdl",0,0).compilerError("Could not match Mix_MusicType to c type");
				}
				MUS->addLocalVariable(PositionID("#sdl",0,0),"_data",T);
				MUS->finalize(PositionID("#sdl",0,0));

				MUS->addLocalFunction(":==")->add(new BuiltinInlineFunction(new FunctionProto(":==",{AbstractDeclaration(MUS),AbstractDeclaration(MUS)},&boolClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					llvm::Value* V1 = MUS->getLocalData(r, id, "_data",instance)->getValue(r, id);
					llvm::Value* V2 = MUS->getLocalData(r, id, "_data",args[0]->evaluate(r))->getValue(r, id);
					return new ConstantData(r.builder.CreateICmpEQ(V1,V2),&boolClass);
				}), PositionID("#sdl",0,0));
				MUS->addLocalFunction(":!=")->add(new BuiltinInlineFunction(new FunctionProto(":!=",{AbstractDeclaration(MUS),AbstractDeclaration(MUS)},&boolClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					llvm::Value* V1 = MUS->getLocalData(r, id, "_data",instance)->getValue(r, id);
					llvm::Value* V2 = MUS->getLocalData(r, id, "_data",args[0]->evaluate(r))->getValue(r, id);
					return new ConstantData(r.builder.CreateICmpNE(V1,V2),&boolClass);
				}), PositionID("#sdl",0,0));
		#define SDL_A(A,B) MUS->staticVariables.addVariable(PositionID("#sdl",0,0),#B, new ConstantData(getRData().builder.CreateInsertValue(llvm::UndefValue::get(MUS->type), llvm::ConstantInt::get(T->type,A,false), 0),MUS));
				SDL_A(MUS_CMD, CMD);
				SDL_A(MUS_NONE, NONE);
				SDL_A(MUS_WAV, WAV);
				SDL_A(MUS_MOD, MOD);
				SDL_A(MUS_MID, MID);
				SDL_A(MUS_OGG, OGG);
				SDL_A(MUS_MP3, MP3);
				SDL_A(MUS_MP3_MAD, MP3_MAD);
				SDL_A(MUS_FLAC, FLAC);
				SDL_A(MUS_MODPLUG, MODPLUG);
		#undef SDL_A
		auto F = Mix_LoadWAV_RW;
	} {
		auto R = opendir;
	}
}




#endif /* OCLASS_HPP_ */
