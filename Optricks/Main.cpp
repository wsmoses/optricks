/*
 * Main.cpp
 *
 *  Created on: Jan 3, 2014
 *      Author: Billy
 */
//TODO allow returning references, but forbid returning local reference
//TODO add bigint/bigfloat with reference counting / gmp/mpfr
//TODO FIX typeof w/ templates
//TODO create website that finds best price considering sales tax / student discount /etc
//TODO nullptr/choice{vector}
/*
 *
def r(int a){
    println(a);
    return a;
}

def hi(int a, int& b, int% c){
    println(typeof(a));
    println(typeof(b));
    println(typeof(c));
    return a + b + c();
}

int b = 17;

hi(r(12),&b,r(35))
 *
 */
#include "./language/Post.hpp"
#include "./parse/Lexer.hpp"
#include "./language/ffi/F_Class.hpp"

#define VERSION 0.4.0
#define VERSION_STRING STR(VERSION)

struct __attribute__((__packed__)) TYPE_DATA {
	void* first;
	const AbstractClass* second;
};

void execF(Lexer& lexer, OModule* mod, Statement* n){
	if(n==NULL) return;// NULL;
	if(n->getToken()==T_IMPORT){
		ImportStatement* import = (ImportStatement*)n;
		char cwd[1024];
		if(getcwd(cwd,sizeof(cwd))==NULL) import->error("Could not getCWD");
		String dir, file;
		getDir(import->toImport, dir, file);
		if(chdir(dir.c_str())!=0) import->error("Could not change directory to "+dir+"/"+file);
		//TODO PREVENT THIS FROM BEING MAIN...............
		lexer.execFiles(true,{file}, nullptr, -2);
		if(chdir(cwd)!=0) import->error("Could not change directory back to "+String(cwd));
		return;
	}
	//if(debug && n->getToken()!=T_VOID) std::cout << n << endl << flush;
	n->registerClasses();
	n->registerFunctionPrototype(getRData());
	n->buildFunction(getRData());
	//const AbstractClass* retType = n->getReturnType();
	//n->checkTypes();
	llvm::FunctionType* FT;

	if(n->getReturnType()->layout==POINTER_LAYOUT)
		FT = llvm::FunctionType::get(VOIDTYPE, llvm::SmallVector<llvm::Type*,1>(1,llvm::PointerType::getUnqual(CLASSTYPE)), false);
	else
		FT = llvm::FunctionType::get(VOIDTYPE, llvm::SmallVector<llvm::Type*,0>(0), false);

	llvm::Function* F = getRData().CreateFunction(":input_",FT,EXTERN_FUNC);
	llvm::BasicBlock* BB = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", F);
	getRData().builder.SetInsertPoint(BB);
	const Data* dat = n->evaluate(getRData());
	assert(dat);
	if(dat->type==R_INT){
		IntLiteral* i = (IntLiteral*)dat;
		i->toStream(std::cout);
		std::cout << endl << flush;
		F->eraseFromParent();
		return;
	} else if(dat->type==R_FLOAT){
		FloatLiteral* i = (FloatLiteral*)dat;
		i->toStream(std::cout);
		std::cout << endl << flush;
		F->eraseFromParent();
		return;
	} else if(dat->type==R_IMAG){
		ImaginaryLiteral* i = (ImaginaryLiteral*)dat;
		i->toStream(std::cout);
		std::cout << endl << flush;
		F->eraseFromParent();
		return;
	} else if(dat->type==R_STR){
		StringLiteral* i = (StringLiteral*)dat;
		std::cout << "'";
		for(const auto & a:i->value){
			if(a==0) std::cout << "\\0";
			else if(a=='\'') std::cout << "\\'";
			else std::cout << a;
		}
		F->eraseFromParent();
		std::cout << "'" << endl << flush;
		return;
	} /*else if(dat->type==R_ARRAY){
			ArrayData* i = (ArrayData*)dat;
			std::cout << "[";
			bool first = true;
			for(const auto & a:i->inner){
				if(first) first = false;
				else std::cout << ", ";
				else if(a=='\'') std::cout << "\\'";
				else std::cout << a;
			}
			std::cout << "'" << endl << flush;
			return;
		}*/

	const AbstractClass* retType = (dat->type==R_VOID)?(&voidClass):(dat->getReturnType());
	if(retType->classType==CLASS_REF){
		retType = ((ReferenceClass*)retType)->innerType;
		dat = ((ReferenceData*)dat)->value;
	}
	if(n->getToken()==T_FUNC || n->getToken()==T_CLASS || n->getToken()==T_DECLARATION){
		retType = &voidClass;
		getRData().builder.CreateRetVoid();
	} else {
		if(retType==TIME_CLASS || retType->classType==CLASS_HASHMAP || retType->classType==CLASS_ARRAY || retType->classType==CLASS_PRIORITYQUEUE){
			LANG_M.getFunction(PositionID(0,0,"<interpreter.main>"), "println", NO_TEMPLATE, {retType}).first->callFunction(getRData(),PositionID(0,0,"<interpreter.main>"), {dat}, nullptr);
			retType = &voidClass;
			getRData().builder.CreateRetVoid();
		} else if(retType->layout==POINTER_LAYOUT){
			llvm::SmallVector<llvm::Type*,2> TYPES(2);
			TYPES[0] = retType->type;
			TYPES[1] = CLASSTYPE;
			auto ST = llvm::StructType::get(llvm::getGlobalContext(),TYPES,true);
			llvm::FunctionType* FT = llvm::FunctionType::get(ST, llvm::SmallVector<llvm::Type*,1>(1,llvm::PointerType::getUnqual(CLASSTYPE)), false);
			F->mutateType(llvm::PointerType::getUnqual(FT));

			llvm::Value* V = dat->getValue(getRData(),PositionID(0,0,"<interpreter.main>"));

			RData& r = getRData();

			auto STAR = r.builder.GetInsertBlock();

			llvm::BasicBlock* END = r.CreateBlockD("is_null", F);
			llvm::BasicBlock* NOT_NULL = r.CreateBlockD("not_null", F);

			r.builder.CreateCondBr(r.builder.CreateIsNull(V),END,NOT_NULL);
			r.builder.SetInsertPoint(NOT_NULL);

			assert(V->getType());
			assert(V->getType()->isPointerTy());
			assert(((llvm::PointerType*)V->getType())->getElementType()->isStructTy());
			assert(((llvm::StructType*)((llvm::PointerType*)V->getType())->getElementType())->getStructNumElements()>=2);
			r.builder.CreateStore(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1)),F->arg_begin());
			r.builder.CreateBr(END);
			r.builder.SetInsertPoint(END);

			r.builder.CreateRet(V);

		} else if(retType->classType!=CLASS_VOID){
			llvm::FunctionType* FT = llvm::FunctionType::get(retType->type, llvm::SmallVector<llvm::Type*,0>(0), false);
			F->mutateType(llvm::PointerType::getUnqual(FT));
			getRData().builder.CreateRet(dat->getValue(getRData(),PositionID(0,0,"<interpreter.main>")));
#ifndef NDEBUG
	if(F->getReturnType()!=retType->type){
		F->getReturnType()->dump();
		retType->type->dump();
		cerr << "Mismatched types: void " << retType->getName() << endl << flush;
		assert(retType->classType==CLASS_VOID);
	}
#endif
	assert(F->getReturnType()==retType->type);
		} else getRData().builder.CreateRetVoid();
	}
	getRData().FinalizeFunction(F);

	void *FPtr = getRData().getExec()->getPointerToFunction(F);

	//TODO introduce new error literal


	if(retType->classType==CLASS_FUNC){
		void* (*FP)() = (void* (*)())(intptr_t)FPtr;
		std::cout << retType->getName() << "(" << FP() << ")" << endl << flush;
	}else if(retType->classType==CLASS_CLASS){
		void* (*FP)() = (void* (*)())(intptr_t)FPtr;
		FP();
		AbstractClass* cp = (AbstractClass*)(FP());
		if(cp)
			std::cout <<  "class{" << cp << ", '"<< cp->getName() << "'}" << endl << flush;
	}else if(retType->classType==CLASS_VOID){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		std::cout << flush;
	} else if(retType->classType==CLASS_FLOAT){
		auto w = ((const FloatClass*)retType)->getWidth();
		if(w==8*sizeof(float)){
			float (*FP)() = (float (*)())(intptr_t)FPtr;
			float t = FP();
			std::cout << t << endl << flush;
		} else if(w==8*sizeof(double)){
			double (*FP)() = (double (*)())(intptr_t)FPtr;
			double t = FP();
			std::cout << t << endl << flush;
		} else if(w==8*sizeof(long double)){
			long double (*FP)() = (long double (*)())(intptr_t)FPtr;
			long double t = FP();
			std::cout << t << endl << flush;
		}
		else {
			((void* (*)())(intptr_t)FPtr)();
			cerr << "Unknown print function for type " << retType->getName() << " with width " << ((const FloatClass*)retType)->getWidth() << " " << (8*sizeof(long double))  << endl << flush;
		}
	}
	else if(retType->classType==CLASS_INT){
		switch(((const IntClass*)retType)->getWidth()){
		case 8:{
			int8_t (*FP)() = (int8_t (*)())(intptr_t)FPtr;
			int8_t t = FP();
			std::cout << (int32_t)t << endl << flush;
			break;
		}
		case 16:{
			int16_t (*FP)() = (int16_t (*)())(intptr_t)FPtr;
			int16_t t = FP();
			std::cout << t << endl << flush;
			break;
		}
		case 32:{
			int32_t (*FP)() = (int32_t (*)())(intptr_t)FPtr;
			int32_t t = FP();
			std::cout << t << endl << flush;
			break;
		}
		case 64:{
			int64_t (*FP)() = (int64_t (*)())(intptr_t)FPtr;
			int64_t t = FP();
			std::cout << t << endl << flush;
			break;
		}
		default:{
			((void* (*)())(intptr_t)FPtr)();
			cerr << "Unknown print function for type " << retType->getName() << " " << str(dat->type) << endl << flush;
		}
		}
	}
	else if(retType->classType==CLASS_BOOL){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		auto t = FP();
		std::cout << (t?"true\n":"false\n") << flush;
	} else if(retType->classType==CLASS_CHAR){
		auto (*FP)() = (char (*)())(intptr_t)FPtr;
		auto t = FP();
		printf("\'%c\'",t);
		std::cout << endl << flush;
	} else if(retType->classType==CLASS_STR){
		auto (*FP)() = (char* (*)())(intptr_t)FPtr;
		auto t = FP();
		printf("\"%s\"",t);
		std::cout << endl << flush;
	} else if(retType->classType==CLASS_CSTRING){
		auto (*FP)() = (char* (*)())(intptr_t)FPtr;
		char* t = FP();
		std::cout << t << endl << flush;
	} else if(retType->classType==CLASS_CPOINTER){
		auto (*FP)() = (void* (*)())(intptr_t)FPtr;
		void* t = FP();
		std::cout << t << endl << flush;
	} else if(retType->classType==CLASS_ENUM){
		auto (*FP)() = (int32_t (*)())(intptr_t)FPtr;
		int32_t t = FP();
		auto EC = ((const EnumClass*)retType);
		for(unsigned i=0; i<EC->names.size(); i++){
			if(EC->names[i].first==t){
				std::cout << EC->names[i].second << endl << flush;
				break;
			}
		}
	}
	/*else if(n->returnType==stringClass){
		StringStruct (*FP)() = (StringStruct (*)())(intptr_t)FPtr;
		auto t = FP();
		String temp(t.data, t.length);
		std::cout << temp << endl << flush;
	} */ else if(retType->classType==CLASS_MATHLITERAL){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		FP();
		switch(((const MathConstantClass*)retType)->mathType){
		case MATH_PI:
			std::cout << "Pi" << endl << flush; break;
		case MATH_E:
			std::cout << "E" << endl << flush; break;
		case MATH_EULER_MASC:
			std::cout << "EulerGamma" << endl << flush; break;
		case MATH_LN2:
			std::cout << "Log2" << endl << flush; break;
		case MATH_CATALAN:
			std::cout << "Catalan" << endl << flush; break;
		}
	} else if(retType->classType==CLASS_NULL){
		void* (*FP)() = (void* (*)())(intptr_t)FPtr;
		FP();
		std::cout << "null" << endl << flush;
	} else if(retType->layout==PRIMITIVEPOINTER_LAYOUT){
		void* (*FP)() = (void* (*)())(intptr_t)FPtr;
		auto t = FP();
		std::cout << retType->getName() << "<" << t << ">" << endl << flush;
	} else if(retType->layout==POINTER_LAYOUT){
		void* (*FP)(const AbstractClass*&) = (void* (*)(const AbstractClass*&))(intptr_t)FPtr;
		const AbstractClass* cla;
		void* data = FP(cla);
		if(data==nullptr)
			std::cout << "null" << endl << flush;
		else{
			std::cout << cla->getName() << "<" << data << ">" << endl << flush;
		}
	} else{
		((void* (*)())(intptr_t)FPtr)();
		cerr << "Unknown print function for type " << retType->getName() << " " << str(dat->type) << endl << flush;
	}
	assert(F);
	F->eraseFromParent();
}
/**
 * Returns whether s starts with b;
 */
bool startsWith(String s, String b){
	if(s.length() < b.length()) return false;
	for(unsigned int i = 0; i<b.length(); i++){
		if(s[i]!=b[i]) return false;
	}
	return true;
}
bool startsWithEq(String s, String b){
	if(s==b) return true;
	b+="=";
	if(s.length() < b.length()) return false;
	for(unsigned int i = 0; i<b.length(); i++){
		if(s[i]!=b[i]) return false;
	}
	return true;
}
String testString(String toTest, String testing){
	String tmp = "";
	if(startsWith(toTest, testing+"=")) tmp = toTest.substr(testing.length()+1);
	if(tmp.length()==0){
		cerr << "Unknown value of " << tmp << " for variable " << toTest << " -- no operation" << endl << flush;
		exit(1);
	}
	return tmp;
}
bool testFor(String toTest, String testing){
	if(toTest==testing) return true;
	if(!startsWith(toTest, testing+"=")) return false;
	String tmp = toTest.substr(testing.length()+1);
	if(tmp=="y" || tmp=="yes") return true;
	else if(tmp=="n" || tmp=="no") return false;
	else{
		cerr << "Unknown value of " << tmp << " for variable " << toTest << " you must use yes/no" << endl << flush;
		exit(1);
	}
}
int main(int argc, char** argv){
	//PlaySound("dance.wav",nullptr,SND_FILENAME | SND_ASYNC);
	LANG_M.addFunction(PositionID(0,0,"#str"),"assert")->add(
			new BuiltinInlineFunction(
					new FunctionProto("assert",{AbstractDeclaration(LazyClass::get(&boolClass))},&voidClass),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		if(!getRData().enableAsserts) return &VOID_DATA;
		std::vector<const Evaluatable*> EV;
		const Data* D = args[0]->evaluate(r)->callFunction(r,id,EV,nullptr);
		assert(D->getReturnType()->classType==CLASS_BOOL);
		llvm::Value* V = D->getValue(r, id);
		if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
			if(! C->isOne()){
				id.fatalError("Assertion failed");
			}
			return &VOID_DATA;
		}
		llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
		llvm::BasicBlock *ThenBB = r.CreateBlock("then",StartBB);
		llvm::BasicBlock *MergeBB = r.CreateBlock("ifcont",StartBB);
		r.builder.CreateCondBr(V, MergeBB,ThenBB);

		r.builder.SetInsertPoint(ThenBB);
		std::stringstream ss;
		ss << "Assertion failed at " << id.fileName << " on line " << id.lineN << " character " << id.charN << "\n";
		auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
		for(const auto& a: ss.str()){
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
		}
		CU = r.getExtern("exit", &voidClass, {&c_intClass});
		r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, 1,false));
		r.builder.CreateUnreachable();
		r.builder.SetInsertPoint(MergeBB);
		return &VOID_DATA;
	}), PositionID(0,0,"#int"));

	LANG_M.addFunction(PositionID(0,0,"#str"),"typeof")->add(
			new BuiltinInlineFunction(
					new FunctionProto("typeof",{AbstractDeclaration(LazyClass::get(&voidClass))},&classClass),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> const Data*{
		assert(args.size()==1);
		const AbstractClass* a=args[0]->getReturnType();
		assert(a->classType==CLASS_LAZY);
		a = ((LazyClass*)a)->innerType;
		return a;
		//const Data* D = args[0]->evaluate(r);
	}), PositionID(0,0,"#int"));
	LANG_M.addFunction(PositionID(0,0,"#str"),"sizeof")->add(
			new BuiltinInlineFunction(
					new FunctionProto("sizeof",{AbstractDeclaration(LazyClass::get(&voidClass))},&intLiteralClass),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> const Data*{
		assert(args.size()==1);
		const AbstractClass* a = args[0]->getReturnType();
		assert(a->classType==CLASS_LAZY);
		a = ((LazyClass*)a)->innerType;
		if(a->classType==CLASS_CLASS){
			const Data* d = args[0]->evaluate(r);
			assert(d->type==R_LAZY);
			a = ((LazyWrapperData*)d)->value->evaluate(r)->getMyClass(r, id);
		}
		uint64_t s = getRData().dlayout->getTypeAllocSize(a->type);
		return new IntLiteral(s);
		//const Data* D = args[0]->evaluate(r);
	}), PositionID(0,0,"#int"));

	String file = "";
	String command = "";
	String output = "";
	int outputFormat = 0; // 0 is default, 1 is IR, 2 is assembly
	bool interactive = false;
	bool forceInt = false;
	for(int i = 1; i<argc; ++i){
		String s = String(argv[i]);
		if(startsWithEq(s, "--debug")){
			getRData().debug = testFor(s,"--debug");
		}
		else if(s=="-ir" || s=="--ir") {
			if(outputFormat!=0){ cerr << "Error: output file already set when trying to set format as LLVM-IR" << endl << flush; exit(1); }
			outputFormat=1;
		} else if(s=="-asm" || s=="--asm") {
			if(outputFormat!=0){ cerr << "Error: output file already set when trying to set format as Assembly" << endl << flush; exit(1); }
			outputFormat=2;
		} else if(s=="-i") { forceInt = true; interactive = true; }
		else if(s=="--help" || s=="-h"){
			std::cout <<
				"Usage: gcc.exe [options] file...\n"
				"Options:\n"
				"   -asm --asm               Emit Assembly instead of executable\n"
				"   -ir --ir                 Emit LLVM IR instead of executable\n"
				"   -i                       Force an interactive session\n"
				"   --interactive=yes|no     Force an interactive session on or off, default yes if no file or command otherwise no\n"
				"   --assert=yes|no          Enable the evaluation of an assertion default=no\n"
				"   --pnacl=yes|no           Emit a PNACL executable or compatible IR\n"
				"   --command='...'          Runs the following command instead of a file\n"
				"   -c ...                   Runs the following command instead of a file\n"
				"   --help                   Display this information\n"
				"   --version                Display compiler version information\n"
				"   -output=<file>           Place the output into <file>\n"
				"   -o <file>                Place the output into <file>\n"
				"\n"
				"For bug reporting instructions, please see:\n"
				"  <http://optricks.xvm.mit.edu/>." << flush;
			return 0;
		} else if(s=="--version"){
			std::cout << "Optricks version " << VERSION_STRING << endl << flush;
			std::cout << "Created by Billy Moses" << endl << endl << flush;
			return 0;
		}
		else if(startsWithEq(s, "--pnacl")){
			getRData().enablePNACL = testFor(s,"--pnacl");
		}
		else if(startsWithEq(s, "--assert")){
			getRData().enableAsserts = testFor(s,"--assert");
		}
		else if(startsWithEq(s, "--interactive")){
			forceInt = true;
			interactive = testFor(s,"--interactive");
		}
		else if(startsWithEq(s,"--command")){
			s = testString(s, "--command");
			if(file!=""){ cerr << "Error: input file already set to " << file << " when trying to set command as " << s << endl << flush; exit(1); }
			else if(command!="") { cerr << "Error: command already set to " << command << " when trying to set command as " << s << endl << flush; exit(1); }
			command=s;
		}
		else if(s=="-c"){
			i++;
			if(i>=argc){
				cerr << "No file when set with -f "<< endl << flush;
				exit(1);
			}
			s = String(argv[i]);
			if(file!=""){ cerr << "Error: input file already set to " << file << " when trying to set command as " << s << endl << flush; exit(1); }
			else if(command!="") { cerr << "Error: command already set to " << command << " when trying to set command as " << s << endl << flush; exit(1); }
			command=s;
		}
		else if(startsWithEq(s,"--output")){
			s = testString(s, "--output");
			if(output!=""){ cerr << "Error: output file already set to " << output << " when trying to set file as " << s << endl << flush; exit(1); }
			output=s;
		}
		else if(s=="-o"){
			i++;
			if(i>=argc){
				cerr << "No file when set with -o "<< endl << flush;
				exit(1);
			}
			s = String(argv[i]);
			if(output!=""){ cerr << "Error: output file already set to " << output << " when trying to set file as " << s << endl << flush; exit(1); }
			output=s;
		}
		else if(startsWithEq(s,"--file")){
			s = testString(s, "--file");
			if(file!=""){ cerr << "Error: input file already set to " << file << " when trying to set file as " << s << endl << flush; exit(1); }
			else if(command!="") { cerr << "Error: command already set to " << command << " when trying to set file as " << s << endl << flush; exit(1); }
			file=s;
		}
		else if(s=="-f"){
			i++;
			if(i>=argc){
				cerr << "No file when set with -f "<< endl << flush;
				exit(1);
			}
			s = String(argv[i]);
			if(file!=""){ cerr << "Error: input file already set to " << file << " when trying to set file as " << s << endl << flush; exit(1); }
			else if(command!="") { cerr << "Error: command already set to " << command << " when trying to set file as " << s << endl << flush; exit(1); }
			file=s;
		}
		else {
			if(file!=""){ cerr << "Error: input file already set to " << file << " when trying to set file as " << s << endl << flush; exit(1); }
			else if(command!="") { cerr << "Error: command already set to " << command << " when trying to set file as " << s << endl << flush; exit(1); }
			file=s;
		}
	}
	if(!forceInt) interactive = file=="" && command=="";
	//ofstream fout (output);
	String error="";
	llvm::raw_fd_ostream* outStream;
	if(outputFormat!=0 || output.length()>0){
		if(output=="-" || output==""){
			outStream = new llvm::raw_fd_ostream(1, true);
			output = "-";
		}
		else outStream = new llvm::raw_fd_ostream(output.c_str(), error, llvm::sys::fs::OpenFlags::F_None);
		if(error.length()>0){
			cerr << error << endl << flush;
			exit(1);
		}
	} else outStream = nullptr;

	initClasses();

	if(interactive) {
		std::cout << "Optricks version " << VERSION_STRING << endl << flush;
		std::cout << "Created by Billy Moses" << endl << endl << flush;
	}

	Lexer lexer(NULL,interactive?'\n':EOF);

	std::vector<const AbstractClass*> V = {&intClass};
	//initializeBaseFunctions(rdata);
	//initFuncsMeta(rdata);
	std::vector<String> files =
	 		{
				getExecutablePath() +"stdlib/stdlib.opt"
#ifdef USE_SDL
				,getExecutablePath() +"stdlib/sdl.opt"
#endif
#ifdef USE_OPENGL
				,getExecutablePath() +"stdlib/opengl.opt"
#endif
		};
	if(!interactive){
		if(command==""){
			auto t = files.size()-1;
			files.push_back(files[t]);
			files[t]=file;
		}
		else{
			cerr << "Commands not supported yet!" << endl << flush;
			exit(1);
		}
		lexer.execFiles(true,files, outStream,outStream?outputFormat:-1);
		if(outStream){
			outStream->close();
		}
	}
	else{
		lexer.execFiles(true,files, nullptr, -1);
		Statement* n;
		Stream st(file, true);
		lexer.f = &st;
		//std::cout << convertClass(void,&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(bool,&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(char,&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(int,&LANG_M)->getName() << endl << flush;
		//std::cout << "== int " << (convertClass(int,&LANG_M)==&intClass) << endl << flush;
		//std::cout << "== c_int " << (convertClass(int,&LANG_M)==&c_intClass) << endl << flush;
		//std::cout << convertClass(int32_t,&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(void(bool, char),LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(void (*)(bool, char),&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(std::pair<bool, char>,&LANG_M)->getName() << endl << flush;
		//std::cout << convertClass(void (*)(int, char),&LANG_M)->getName() << endl << flush;
		std::cout << START << flush;
		//st.force("{int:long} a\n");
		//st.force("int[] a\n");
		//st.force("complex{int} a;\n");
		/*
		st.force("4/2*3/4\n");
		st.force("extern double cos(double a); cos(3.14159)\n");
		st.force("lambda int a,int b: a+b\n");
		st.force("(lambda int a,int b: a+b)(4,5)\n");
		st.force("(lambda double a: (lambda double sq: sq*sq)(sin(a))+(lambda double sq: sq*sq)(cos(a)))(.9)\n");
		st.force("2+3.1\n");
		st.force("if true: putchar(71); else: putchar(72)\n");
		st.force("(lambda int a, int b, int c: if a<=b & a<=c: a; elif b<=a & b<=c: b; else: c)(1,2,3)\n");
		st.force("(lambda int a, int b, int c: if a<=b & a<=c putchar(a) elif b<=a & b<=c putchar(b) else putchar(c))(1,2,3)\n");
		st.force("if(true){ putchar(71); putchar(72); }\n");
		st.force("true?1:0\n");
		st.force("if(true){ int i = 74; putchar(i); i = 75; putchar(i); }\n");
		st.force("def int printr(int i){ int j = i+48; putchar(j); return i;}\n");
		st.force("def int printr(int i){ int j = i+48; putchar(j); return i;}; printr(9);\n");
		st.force("def int p2(int i){ if(i>0) p2(i-1); return putchar(i+48); }; printr(9);\n");
		st.force("def void hi(){ putchar(50); return; }");
		//*/
		//		st.force("if(true){ printr(9); def int printr(int i){ int j = i+48; putchar(j); return i;}; }\n"); //TODO allow
		//st.force("for(int i = 0; i<10; i= i+1) putchar(i+48)\n");
		//st.force("if(true){ for(int i = 0; i<10; i= i+1) putchar(i+48)} \n");
		//st.force("for(int i = 0; i<10; i+=1){ if(i==5) break; printi(i); }\n");
		//st.force("(def void (int i){for(int i = 0; i<10; i+=1){ if(i%2==0) continue; printi(i) } })(0);");
		//st.force("(def void () printi(72))()\n");
		//st.force("for(int i = 0; i<1000 i+=1) printd((def double (int i){ double a=1 auto b=a for(int j=3 j<=i j+=1){ auto tmp = a+b a = b b = tmp} return b})(i))\n");
		//st.force("for(int i = 0; i<1000 i+=1) printd((def auto (int i){ double a=1 auto b=a for(int j=3 j<=i j+=1){ auto tmp = a+b a = b b = tmp} return b})(i))\n");
		//st.force("\"hi\"\n"); debug = true;
		//st.force("(lambda complex a: a.real)(complex())\n");
		//st.force("(complex()).real\n");
		//st.force("(def complex (complex a){ a.real = 3; return a})(complex())\n");
		//st.force("def complex operator/(complex c, complex b){ return c; }\n");
		//st.force("complex()*complex()\n");
		//st.force("(lambda string s: s.length)('c')\n");
		//st.force("int i=7;\n");
		//st.force("(def void dgah(){print(hi); })()\n");
		//st.force("'hello'[0];\n");
		//st.force("for i in range(,10): print(i);\n");

		//TODO (global)
		//st.force("(int,int) a; a = (3,4); print(a._0);\n");
		//st.force("int[] ar=[3,1,4,1,5,9,2,6];qsort(ar.carr,8,4,lambda int& a, int& b: lang.c.int(a-b));\n");
		//st.force("(byte,byte)\n");
		while(true){
			st.enableOut = true;
			st.trim(EOF);
			n = lexer.getNextStatement('\n',true);
			st.enableOut = false;
			bool first = true;
			while(n->getToken()!=T_VOID){
				first = false;
				execF(lexer,lexer.myMod, n);
				st.done = false;
				if(st.last()=='\n' || st.peek()=='\n') break;
				bool reset = false;
				while(true){
					char c = st.peek();
					if(c==';') st.read();
					else if(isWhitespace(c)){
						if(c=='\n'){
							reset = true;
							break;
						}
						else st.read();
					} else break;
				} if(reset) break;
				st.done = false;
				st.enableOut = true;
				n = lexer.getNextStatement('\n',true);
				st.enableOut = false;
			}
			st.done = false;
			if(st.last()==EOF) break;
			std::cout << START << flush;
			while(st.peek()=='\n' || st.peek()==';') st.read();
			st.done = false;

			st.done = false;
			if(first) break;
		}
	}
	return 0;
}
