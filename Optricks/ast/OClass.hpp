/*
 * OClass.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "../language/class/UserClass.hpp"
#include "../language/class/EnumClass.hpp"
#include "../language/class/builtin/NullClass.hpp"
#include "../language/class/builtin/PriorityQueueClass.hpp"
#include "../language/class/builtin/WrapperClass.hpp"
#include "../language/ffi/F_Class.hpp"
#include "../language/ffi/F_Function.hpp"
#include "../language/ffi/F_Var.hpp"

UserClass* TIME_CLASS;
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

		void reset() const override final{}

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
				if(proto->layout!=PRIMITIVEPOINTER_LAYOUT)
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
	ArrayClass::get(nullptr);
	convertClass(int,&LANG_M);
	convertClass(short,&LANG_M);
	convertClass(long,&LANG_M);
	convertClass(long long,&LANG_M);
	//convertClass(size_t,&LANG_M);
	convertClass(int64_t,&LANG_M);
	//add_import_c_function(&LANG_M, malloc);

	NS_LANG_C.staticVariables.addFunction(PositionID(0,0,"#init"),"clock")->add(
			new ExternalFunction(new FunctionProto("clock", std::vector<AbstractDeclaration>(), convertClass(clock_t,&LANG_M))),
	PositionID(0,0,"#init"));

	add_import_c_const(&(NS_LANG_C.staticVariables), CLOCKS_PER_SEC);

	NS_LANG_C.staticVariables.addFunction(PositionID(0,0,"#init"),"srand")->add(
			new ExternalFunction(new FunctionProto("srand", {AbstractDeclaration(&c_intClass)}, &voidClass)),
	PositionID(0,0,"#init"));

	LANG_M.addFunction(PositionID(0,0,"#class"),"seed")->add(
		new BuiltinInlineFunction(new FunctionProto("seed",{AbstractDeclaration(&intClass)},&voidClass),
		[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		r.seed(args[0]->evalV(r,id));
		return &VOID_DATA;
	}), PositionID(0,0,"#int"));

	LANG_M.addFunction(PositionID(0,0,"#class"),"rand")->add(
		new BuiltinInlineFunction(new FunctionProto("rand",std::vector<AbstractDeclaration>(),&intClass),
		[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==0);
		return new ConstantData(r.rand(), &intClass);
	}), PositionID(0,0,"#int"));

	LANG_M.addFunction(PositionID(0,0,"#class"),"randInt")->add(
			new BuiltinInlineFunction(new FunctionProto("randInt",{AbstractDeclaration(&intClass)},&intClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* MAX = args[0]->evalV(r,id);
			return new ConstantData(r.randInt(MAX), &intClass);
		}), PositionID(0,0,"#int"));
	add_import_c_const(&(NS_LANG_C.staticVariables), RAND_MAX);

	auto Stream = new UserClass(&LANG_M,"Stream",nullptr,PRIMITIVE_LAYOUT,true);
	Stream->addLocalVariable(PositionID("#stream",0,0),"descriptor",&c_intClass);
	Stream->finalize(PositionID("#stream",0,0));
	LANG_M.addClass(PositionID("#stream",0,0),Stream);
	Stream->addLocalFunction("printf", PositionID("#stream",0,0),new BuiltinInlineFunction(new FunctionProto("printf",{AbstractDeclaration(Stream),AbstractDeclaration(&stringLiteralClass)},&intClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()>=1);
		//TODO custom formatting for printf (and checks for literals / correct format / etc)
		const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
		llvm::SmallVector<llvm::Value*,2> m_args(args.size());
		m_args[0] = Stream->getLocalData(r, id, "descriptor", instance)->getValue(r, id);
		m_args[1] = r.getConstantCString(value);
		assert(m_args[1]);
		for(unsigned i=1; i<args.size(); i++){
			m_args[i+1] = args[i]->evalV(r, id);
			assert(m_args[i+1]);
		}
		return new ConstantData(r.builder.CreateSExtOrTrunc(r.dprintf<2>(m_args), INT32TYPE), &intClass);
	}), false);

	//add_import_c_function(&LANG_M, qsort);
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
			return ArrayClass::get(args[0]);
		}),"array");

	LANG_M.addClass(PositionID(0,0,"#priorityqueue"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
			//if(args.size()==0) return ComplexClass::get(&doubleClass);
			if(args.size()!=1){
				id.error("Must use template class 'PriorityQueue' with one argument");
				return nullptr;
			}
			return PriorityQueueClass::get(args[0]);
		}),"PriorityQueue");
	LANG_M.addClass(PositionID(0,0,"#map"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
			//if(args.size()==0) return ComplexClass::get(&doubleClass);
			if(args.size()!=2){
				id.error("Must use template class 'map' with one argument");
				return nullptr;
			}
			return HashMapClass::get(args[0], args[1]);
		}),"map");
	LANG_M.addClass(PositionID(0,0,"#tuple"),new BuiltinClassTemplate([](RData& r,PositionID id,const std::vector<const AbstractClass*>& args) -> const AbstractClass*{
		if(args.size()==0) {
			id.error("Must use template class 'tuple' with at least one argument - found "+str(args.size()));
			return nullptr;
			}
		std::vector<const AbstractClass*> ar;
		for(unsigned i=0; i<args.size(); i++)
			ar.push_back(args[i]);
			return TupleClass::get(ar);
		}),"tuple");
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

	NS_LANG_C.staticVariables.addFunction(PositionID(0,0,"#str"),"scanf")->add(
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
#ifdef USE_SDL
	auto SDL = new ScopeClass(&LANG_M, PositionID("#init",0,0), "sdl");
	{
		auto AF = new UserClass(&SDL->staticVariables,"AudioFormat",nullptr,PRIMITIVE_LAYOUT,true,false,llvm::IntegerType::get(llvm::getGlobalContext(),8*sizeof(SDL_AudioFormat)));
		SDL->staticVariables.addClass(PositionID("#sdl",0,0),AF);
		AF->constructors.add(new BuiltinInlineFunction(new FunctionProto("AudioFormat",{AbstractDeclaration(&shortClass)},AF),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* A = args[0]->evalV(r, id);
			return new ConstantData(A, AF);
		}), PositionID("#sdl",0,0));
		AF->addLocalFunction("isFloat", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isFloat",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_DATATYPE)),BOOLTYPE),&boolClass);
		}), false);
		AF->addLocalFunction("isBigEndian", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isBigEndian",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_ENDIAN)),BOOLTYPE),&boolClass);
		}), false);
		AF->addLocalFunction("isSigned", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isSigned",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_SIGNED)),BOOLTYPE),&boolClass);
		}), false);
		AF->addLocalFunction("isInt", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isInt",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_DATATYPE)),BOOLTYPE)),&boolClass);
		}), false);
		AF->addLocalFunction("isLittleEndian", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isLittleEndian",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_ENDIAN)),BOOLTYPE)),&boolClass);
		}), false);
		AF->addLocalFunction("isUnsigned", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("isUnsigned",{AbstractDeclaration(AF)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateNot(r.builder.CreateTrunc(r.builder.CreateLShr(V, log2(SDL_AUDIO_MASK_SIGNED)),BOOLTYPE)),&boolClass);
		}), false);
		AF->addLocalFunction("bitSize", PositionID("#sdl",0,0), new BuiltinInlineFunction(new FunctionProto("bitSize",{AbstractDeclaration(AF)},&byteClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			assert(instance);
			llvm::Value* V = instance->getValue(r, id);
			return new ConstantData(r.builder.CreateTrunc(r.builder.CreateAnd(V, llvm::ConstantInt::get(shortClass.type,SDL_AUDIO_MASK_BITSIZE)),byteClass.type),&byteClass);
		}), false);
#define SDL_A(A,B) AF->staticVariables.addVariable(PositionID("#sdl",0,0),#B, new ConstantData(llvm::ConstantInt::get(AF->type,A,false),AF));
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
		std::vector<std::pair<int,String>> E_D;
		#define SDL_A(A,B) E_D.push_back(std::pair<int,String>(A,#B));
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
		auto MUS = new EnumClass(&SDL->staticVariables,"MusicType",E_D,PositionID("#sdl",0,0),llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(Mix_MusicType)));
		SDL->staticVariables.addClass(PositionID("#sdl",0,0),MUS);
	}
#endif
	{


#if (defined(WIN32) || defined(_WIN32))
		auto FD = new UserClass(&LANG_M,"SoundFlag",nullptr,PRIMITIVE_LAYOUT,false);
		LANG_M.addClass(PositionID("#dir",0,0),FD);
#define ADD(A) FD->staticVariables.addVariable(PositionID("#dir",0,0),#A,new ConstantData(getInt32(SND_##A),&intClass));
		ADD(APPLICATION);
		ADD(ALIAS);
		ADD(ALIAS_ID);
		ADD(ASYNC);
		ADD(FILENAME);
		ADD(LOOP);
		ADD(MEMORY);
		ADD(NODEFAULT);
		ADD(NOSTOP);
		ADD(NOWAIT);
		ADD(PURGE);
		ADD(RESOURCE);
//		ADD(SENTRY);
		ADD(SYNC);
//		ADD(SYSTEM);
#undef ADD
		FD->finalize(PositionID("#dir",0,0));
#endif

		LANG_M.addFunction(PositionID("#dir",0,0),"listdir")->add(new BuiltinCompiledFunction(new FunctionProto("listdir",{AbstractDeclaration(&c_stringClass)},ArrayClass::get(&c_stringClass)),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> const Data*{
			assert(args.size()==1);
			auto ARRAY_DATA = ArrayClass::get(&c_stringClass)->callFunction(r, id, std::vector<const Evaluatable*>(),nullptr);


			llvm::Value* const s_value = args[0]->evalV(r, id);
			auto mlen_0 = r.strlen(s_value);
			auto end = r.builder.CreateICmpEQ(r.builder.CreateLoad(r.builder.CreateGEP(s_value, r.builder.CreateSub(mlen_0, getSizeT(1)))), CharClass::getValue('/'));
			auto mlen = r.builder.CreateAdd(mlen_0,r.builder.CreateSelect(end, getSizeT(1),getSizeT(2)));
			auto dir = r.opendir(s_value,id);
			auto LOOP = r.builder.GetInsertBlock();
			assert(LOOP->empty());
			LOOP->setName("loop");
			llvm::Function* FUNC = LOOP->getParent();
			//llvm::BasicBlock* LOOP = r.CreateBlockD("loop", FUNC);
			auto NOTNULL = r.CreateBlockD("notnull",FUNC);
			auto IS_FILE = r.CreateBlockD("is_file",FUNC);
			auto NOT_FILE = r.CreateBlockD("not_file",FUNC);
			auto DONE = r.CreateBlockD("done", FUNC);

			r.builder.SetInsertPoint(LOOP);

			//POSIX_DIR_NEXT_NAME

			struct dirent TMP;
			size_t beforeChar = (size_t)((size_t)(&(TMP.d_name))-(size_t)(&(TMP)));
			//size_t charSize = sizeof(TMP.d_name);
			//size_t afterChar = sizeof(TMP)-beforeChar-charSize;
			auto dirent_p = r.readdir(dir);
			r.builder.CreateCondBr(r.builder.CreateIsNull(dirent_p),DONE,NOTNULL);

			r.builder.SetInsertPoint(NOTNULL);
			auto str = r.builder.CreateConstGEP2_32(dirent_p, 0, beforeChar);

			auto stl = r.strlen(str);
			auto len = r.builder.CreateAdd(mlen, stl);
			auto cp = r.allocate(CHARTYPE, len);
			//END_POSIX_DIR_NEXT_NAME
			auto spf = r.builder.CreateSelect(end, r.getConstantCString("%s%s"),r.getConstantCString("%s/%s"));

			llvm::SmallVector<llvm::Value*,4> V(4);
			V[0] = cp;
			V[1] = spf;
			V[2] = s_value;
			V[3] = str;
			r.sprintf<4>(V);

			auto stat_t = llvm::ArrayType::get(CHARTYPE, sizeof(struct stat));
			auto P = r.builder.CreateAlloca(stat_t);

			llvm::SmallVector<llvm::Type*,2> stat_args(2);
			stat_args[0] = C_STRINGTYPE;
			stat_args[1] = llvm::PointerType::getUnqual(stat_t);

			auto STAT_F = r.getExtern("stat", llvm::FunctionType::get(C_INTTYPE,stat_args,true));

			r.builder.CreateCall2(STAT_F, cp, P);

			struct stat stat_tmp;
			auto P2 = r.builder.CreateConstGEP1_64(r.builder.CreatePointerCast(P, C_POINTERTYPE), (size_t)((size_t)(&(stat_tmp.st_mode))-(size_t)(&(stat_tmp))));
			auto TT = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(stat_tmp.st_mode));
			auto D = r.builder.CreatePointerCast(P2, llvm::PointerType::getUnqual(TT));
			auto D_L = r.builder.CreateLoad(D);
			auto isFil = r.builder.CreateICmpEQ(r.builder.CreateAnd(D_L, llvm::ConstantInt::get(TT, S_IFMT,false)), llvm::ConstantInt::get(TT, S_IFREG,false));

			r.builder.CreateCondBr(isFil, IS_FILE, NOT_FILE);

			r.builder.SetInsertPoint(IS_FILE);
			ConstantData STR_DATA(cp, &c_stringClass);
			//ACTUALLY DO STUFF
			getBinop(r, id, ARRAY_DATA, &STR_DATA,"[]=");
			r.builder.CreateBr(LOOP);

			r.builder.SetInsertPoint(NOT_FILE);
			r.free(cp);
			r.builder.CreateBr(LOOP);

			r.builder.SetInsertPoint(DONE);
			r.closedir(dir);
			return ARRAY_DATA;
		}), PositionID("#dir",0,0));
		std::vector<const AbstractClass*> C_VEC({&c_stringClass});
		LANG_M.addFunction(PositionID("#dir",0,0),"mapdir")->add(new BuiltinCompiledFunction(new FunctionProto("mapdir",{AbstractDeclaration(&c_stringClass),
				AbstractDeclaration(FunctionClass::get(&voidClass,C_VEC))},&voidClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> const Data*{
			assert(args.size()==2);

			llvm::Value* const s_value = args[0]->evalV(r, id);
			auto MYFUNC = args[1]->evaluate(r);
			auto mlen_0 = r.strlen(s_value);
			auto end = r.builder.CreateICmpEQ(r.builder.CreateLoad(r.builder.CreateGEP(s_value, r.builder.CreateSub(mlen_0, getSizeT(1)))), CharClass::getValue('/'));
			auto mlen = r.builder.CreateAdd(mlen_0,r.builder.CreateSelect(end, getSizeT(1),getSizeT(2)));
			auto dir = r.opendir(s_value,id);
			auto LOOP = r.builder.GetInsertBlock();
			assert(LOOP->empty());
			LOOP->setName("loop");
			llvm::Function* FUNC = LOOP->getParent();
			//llvm::BasicBlock* LOOP = r.CreateBlockD("loop", FUNC);
			auto NOTNULL = r.CreateBlockD("notnull",FUNC);
			auto IS_FILE = r.CreateBlockD("is_file",FUNC);
			auto NOT_FILE = r.CreateBlockD("not_file",FUNC);
			auto DONE = r.CreateBlockD("done", FUNC);

			r.builder.SetInsertPoint(LOOP);

			//POSIX_DIR_NEXT_NAME

			struct dirent TMP;
			size_t beforeChar = (size_t)((size_t)(&(TMP.d_name))-(size_t)(&(TMP)));
			//size_t charSize = sizeof(TMP.d_name);
			//size_t afterChar = sizeof(TMP)-beforeChar-charSize;
			auto dirent_p = r.readdir(dir);
			r.builder.CreateCondBr(r.builder.CreateIsNull(dirent_p),DONE,NOTNULL);

			r.builder.SetInsertPoint(NOTNULL);
			auto str = r.builder.CreateConstGEP2_32(dirent_p, 0, beforeChar);

			auto stl = r.strlen(str);
			auto len = r.builder.CreateAdd(mlen, stl);
			auto cp = r.allocate(CHARTYPE, len);
			//END_POSIX_DIR_NEXT_NAME
			auto spf = r.builder.CreateSelect(end, r.getConstantCString("%s%s"),r.getConstantCString("%s/%s"));

			llvm::SmallVector<llvm::Value*,4> V(4);
			V[0] = cp;
			V[1] = spf;
			V[2] = s_value;
			V[3] = str;
			r.sprintf<4>(V);

			auto stat_t = llvm::ArrayType::get(CHARTYPE, sizeof(struct stat));
			auto P = r.builder.CreateAlloca(stat_t);

			llvm::SmallVector<llvm::Type*,2> stat_args(2);
			stat_args[0] = C_STRINGTYPE;
			stat_args[1] = llvm::PointerType::getUnqual(stat_t);

			auto STAT_F = r.getExtern("stat", llvm::FunctionType::get(C_INTTYPE,stat_args,true));

			r.builder.CreateCall2(STAT_F, cp, P);

			struct stat stat_tmp;
			auto P2 = r.builder.CreateConstGEP1_64(r.builder.CreatePointerCast(P, C_POINTERTYPE), (size_t)((size_t)(&(stat_tmp.st_mode))-(size_t)(&(stat_tmp))));
			auto TT = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(stat_tmp.st_mode));
			auto D = r.builder.CreatePointerCast(P2, llvm::PointerType::getUnqual(TT));
			auto D_L = r.builder.CreateLoad(D);
			auto isFil = r.builder.CreateICmpEQ(r.builder.CreateAnd(D_L, llvm::ConstantInt::get(TT, S_IFMT,false)), llvm::ConstantInt::get(TT, S_IFREG,false));

			r.builder.CreateCondBr(isFil, IS_FILE, NOT_FILE);

			r.builder.SetInsertPoint(IS_FILE);
			ConstantData STR_DATA(cp, &c_stringClass);
			//ACTUALLY DO STUFF
			MYFUNC->callFunction(r, id, {&STR_DATA}, nullptr);
			r.builder.CreateBr(LOOP);

			r.builder.SetInsertPoint(NOT_FILE);
			r.free(cp);
			r.builder.CreateBr(LOOP);

			r.builder.SetInsertPoint(DONE);
			r.closedir(dir);
			return &VOID_DATA;
		}), PositionID("#dir",0,0));

		std::vector<std::pair<int,String>> E_D;
#define WINDOWS 0
#define LINUX 1
#define OSX 2
		E_D.push_back(std::pair<int,String>(WINDOWS,"windows"));
		E_D.push_back(std::pair<int,String>(LINUX,"linux"));
		E_D.push_back(std::pair<int,String>(OSX,"osx"));
		auto OS_T = new EnumClass(&LANG_M,"os",E_D,PositionID("#os",0,0),byteClass.type);

#if defined(WIN32) || defined(_WIN32)
#define OS WINDOWS
#elif defined(__APPLE__)
#define OS OSX
#else
#define OS LINUX
#endif
		OS_T->staticVariables.addVariable(PositionID("#os",0,0),"self",new ConstantData(llvm::ConstantInt::get(byteClass.type, OS, false),OS_T));
		LANG_M.addClass(PositionID("#sdl",0,0),OS_T);
#undef OS
#undef LINUX
#undef WINDOWS
#undef OSX
		/*std::vector<std::pair<int,String>> E_D;
		#define SDL_A(A,B) E_D.push_back(std::pair<int,String>(A,#B));
				SDL_A(DT_UNKNOWN, UNKNOWN);
				SDL_A(DT_REG, REGULAR);
				SDL_A(DT_DIR, DIRECTORY);
				SDL_A(DT_FIFO, FIFO);
				SDL_A(DT_SOCK, SOCKET);
				SDL_A(DT_CHR, CHAR);
				SDL_A(DT_BLK, BLOCK);
				SDL_A(DT_LNK, LINK);
		#undef SDL_A
		auto FT = new EnumClass(DIR->staticVariables,"FileType",E_D,PositionID("#dir",0,0),llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(unsigned char)));
		DIR->staticVariables.addClass(PositionID("#sdl",0,0),FT);*/
		TIME_CLASS = new UserClass(&LANG_M,"Time",nullptr,PRIMITIVE_LAYOUT,true,false,llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t)));
		LANG_M.addClass(PositionID("#dir",0,0),TIME_CLASS);
		TIME_CLASS->constructors.add(new BuiltinInlineFunction(
				new FunctionProto("TIME",std::vector<AbstractDeclaration>(),TIME_CLASS),
		[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==0);
		llvm::SmallVector<llvm::Type*,1> time_args(1);
		time_args[0] = llvm::PointerType::getUnqual(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t)));
		auto TIME = r.getExtern("time",llvm::FunctionType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t)),time_args,false));
		auto V = r.builder.CreateCall(TIME, llvm::ConstantPointerNull::get((llvm::PointerType*)time_args[0]));
		return new ConstantData(V, TIME_CLASS);
	}), PositionID(0,0,"#int"));

		TIME_CLASS->addLocalFunction(":<", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":<",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpULT(V1, V2), &boolClass);
			}),false);
		TIME_CLASS->addLocalFunction(":<=", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":<=",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpULE(V1, V2), &boolClass);
			}), false);
		TIME_CLASS->addLocalFunction(":>", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":>",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpUGT(V1, V2), &boolClass);
			}), false);
		TIME_CLASS->addLocalFunction(":>=", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":>",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpUGE(V1, V2), &boolClass);
			}), false);
		TIME_CLASS->addLocalFunction(":==", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":==",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpEQ(V1, V2), &boolClass);
			}), false);

		TIME_CLASS->addLocalFunction(":!=", PositionID("#time",0,0),
				new BuiltinInlineFunction(
						new FunctionProto(":!=",{AbstractDeclaration(TIME_CLASS),AbstractDeclaration(TIME_CLASS)},&boolClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V1 = instance->evalV(r, id);
				auto V2 = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateICmpNE(V1, V2), &boolClass);
			}), false);

		TIME_CLASS->addLocalFunction("hash", PositionID("#time",0,0),
			new BuiltinInlineFunction(
					new FunctionProto("hash",{AbstractDeclaration(TIME_CLASS)},&intClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			auto V1 = instance->evalV(r, id);
			return new ConstantData(r.builder.CreateZExtOrTrunc(V1,INT32TYPE), &intClass);
		}), false);
		LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
				new BuiltinInlineFunction(
						new FunctionProto("print",{AbstractDeclaration(TIME_CLASS)},&voidClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V = args[0]->evaluate(r);
				assert(V->type==R_CONST || V->type==R_LOC);
				llvm::SmallVector<llvm::Type*,1> ctime_args(1);
				ctime_args[0] = llvm::PointerType::getUnqual(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t)));
				auto CTIME = r.getExtern("ctime",llvm::FunctionType::get(C_STRINGTYPE,ctime_args,false));
				auto STR = r.builder.CreateCall(CTIME, ((LLVMData*)V)->toLocation(r,"time")->value->getPointer(r, id));
				auto len = r.strlen(STR);
				r.builder.CreateStore(CharClass::getValue('\0'),r.builder.CreateGEP(STR,r.builder.CreateSub(len, getSizeT(1))));
				r.printf("%s",STR);
				return &VOID_DATA;
			}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#str"),"println")->add(
				new BuiltinInlineFunction(
						new FunctionProto("println",{AbstractDeclaration(TIME_CLASS)},&voidClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					auto V = args[0]->evaluate(r);
					assert(V->type==R_CONST || V->type==R_LOC);
					llvm::SmallVector<llvm::Type*,1> ctime_args(1);
					ctime_args[0] = llvm::PointerType::getUnqual(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t)));
					auto CTIME = r.getExtern("ctime",llvm::FunctionType::get(C_STRINGTYPE,ctime_args,false));
					auto STR = r.builder.CreateCall(CTIME, ((LLVMData*)V)->toLocation(r,"time")->value->getPointer(r, id));
					auto len = r.strlen(STR);
					r.builder.CreateStore(CharClass::getValue('\0'),r.builder.CreateGEP(STR,r.builder.CreateSub(len, getSizeT(1))));
					r.puts(STR);
					return &VOID_DATA;
			}), PositionID(0,0,"#int"));

		auto FileStat = new UserClass(&LANG_M,"FileStat",nullptr,PRIMITIVE_LAYOUT,true,false,llvm::ArrayType::get(CHARTYPE, sizeof(struct stat)));
		LANG_M.addClass(PositionID("#dir",0,0),FileStat);
#define DATA(name, stat_var, returnType)\
		FileStat->addLocalFunction(#name, PositionID("#time",0,0),new BuiltinInlineFunction(new FunctionProto(#name,{AbstractDeclaration(FileStat)},returnType),\
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{\
					assert(args.size()==0);\
					assert(instance->type==R_LOC || instance->type==R_CONST);\
					auto P = ((LLVMData*)instance)->toLocation(r,"this")->value->getPointer(r, id);\
					struct stat stat_tmp;\
					auto P2 = r.builder.CreateConstGEP1_64(r.builder.CreatePointerCast(P, C_POINTERTYPE), (size_t)((size_t)(&(stat_tmp.stat_var))-(size_t)(&(stat_tmp))));\
					auto D = r.builder.CreatePointerCast(P2, llvm::PointerType::getUnqual(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(stat_tmp.stat_var))));\
					return new ConstantData(r.builder.CreateZExtOrTrunc(r.builder.CreateLoad(D), (returnType)->type),returnType);\
				}), false);
		DATA(getDev, st_dev, &intClass);
		DATA(getMode, st_mode, &intClass);
		DATA(getNLink, st_nlink, &intClass);
		DATA(getUID, st_uid, &intClass);
		DATA(getGID, st_gid, &intClass);
		DATA(getSize, st_size, &intClass);
		DATA(getAccessTime, st_atime, TIME_CLASS);
		DATA(getModificationTime, st_mtime, TIME_CLASS);
		DATA(getStatusTime, st_ctime, TIME_CLASS);
#undef DATA

		FileStat->addLocalFunction("isFile", PositionID("#dir",0,0),new BuiltinInlineFunction(new FunctionProto("isFile",{AbstractDeclaration(FileStat)},&boolClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==0);
					assert(instance->type==R_LOC || instance->type==R_CONST);
					auto P = ((LLVMData*)instance)->toLocation(r,"this")->value->getPointer(r, id);
					struct stat stat_tmp;
					auto P2 = r.builder.CreateConstGEP1_64(r.builder.CreatePointerCast(P, C_POINTERTYPE), (size_t)((size_t)(&(stat_tmp.st_mode))-(size_t)(&(stat_tmp))));
					auto TT = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(stat_tmp.st_mode));
					auto D = r.builder.CreatePointerCast(P2, llvm::PointerType::getUnqual(TT));
					return new ConstantData(r.builder.CreateICmpEQ(r.builder.CreateAnd(r.builder.CreateLoad(D), llvm::ConstantInt::get(TT, S_IFMT,false)), llvm::ConstantInt::get(TT, S_IFREG,false)),&boolClass);
				}), false);
		FileStat->addLocalFunction("isDirectory", PositionID("#dir",0,0), new BuiltinInlineFunction(new FunctionProto("isDirectory",{AbstractDeclaration(FileStat)},&boolClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==0);
					assert(instance->type==R_LOC || instance->type==R_CONST);
					auto P = ((LLVMData*)instance)->toLocation(r,"this")->value->getPointer(r, id);
					struct stat stat_tmp;
					auto P2 = r.builder.CreateConstGEP1_64(r.builder.CreatePointerCast(P, C_POINTERTYPE), (size_t)((size_t)(&(stat_tmp.st_mode))-(size_t)(&(stat_tmp))));
					auto TT = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(stat_tmp.st_mode));
					auto D = r.builder.CreatePointerCast(P2, llvm::PointerType::getUnqual(TT));
					return new ConstantData(r.builder.CreateICmpEQ(r.builder.CreateAnd(r.builder.CreateLoad(D), llvm::ConstantInt::get(TT, S_IFMT,false)), llvm::ConstantInt::get(TT, S_IFDIR,false)),&boolClass);
				}), false);
	}
}




#endif /* OCLASS_HPP_ */
