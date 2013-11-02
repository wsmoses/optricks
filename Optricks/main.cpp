#define MAIN_CPP
//TODO automatic constant detection
//TODO final argument detection (e.g. will not be set)
//TODO constant expr
#include "containers/all.hpp"
#include <fstream>
#include <iostream>
#include "Lexer.hpp"
/**
 * TODO create allocation of global memory after function prototype but before built
 */

void execF(Lexer& lexer, OModule* mod, Statement* n,bool debug){
	if(n==NULL) return;// NULL;
	if(n->getToken()==T_IMPORT){
		ImportStatement* import = (ImportStatement*)n;
		char cwd[1024];
		if(getcwd(cwd,sizeof(cwd))==NULL) import->error("Could not getCWD");
		String dir, file;
		getDir(import->toImport, dir, file);
		if(chdir(dir.c_str())!=0) import->error("Could not change directory to "+dir+"/"+file);
		std::vector<String> pl = {file};
		lexer.execFiles(true,pl, NULL,debug,0);
		if(chdir(cwd)!=0) import->error("Could not change directory back to "+String(cwd));
		return;
	}
	if(debug && n->getToken()!=T_VOID) std::cout << n << endl << flush;
	n = n->simplify();
	n->registerClasses(lexer.rdata);
	n->registerFunctionPrototype(lexer.rdata);
	n->buildFunction(lexer.rdata);
	n->checkTypes(lexer.rdata);
	Type* type;
	if(n->returnType==functionClass || n->returnType==classClass || n->getToken()==T_DECLARATION) type=VOIDTYPE;
	else type = n->returnType->getType(lexer.rdata);
	assert(type!=NULL);
	if(n->getToken()!=T_DECLARATION && n->returnType == complexClass){
		n = new E_FUNC_CALL(PositionID(0,0,"#main"), new E_VAR(PositionID(0,0,"#main"), mod->getPointer(PositionID(0,0,"#main"), "printc")), {n});
		n = n->simplify();
		n->registerClasses(lexer.rdata);
		n->registerFunctionPrototype(lexer.rdata);
		n->buildFunction(lexer.rdata);
		n->checkTypes(lexer.rdata);
		type = VOIDTYPE;
	}
	FunctionType *FT = FunctionType::get(type, std::vector<Type*>(), false);
	Function *F = Function::Create(FT, Function::ExternalLinkage, "", lexer.rdata.lmod);
	BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
	lexer.rdata.builder.SetInsertPoint(BB);
	DATA dat = n->evaluate(lexer.rdata);
//	Value* v = dat.getValue(lexer.rdata);
	if( type!=VOIDTYPE)
		lexer.rdata.builder.CreateRet(dat.getValue(lexer.rdata));
	else
		lexer.rdata.builder.CreateRetVoid();
	//cout << "testing cos" << cos(3) << endl << flush;
	if(debug){
	F->dump();
	cerr << flush;
	}
	verifyFunction(*F);
	//cout << "verified" << endl << flush;
	lexer.rdata.fpm->run(*F);
	//cout << "fpm" << endl << flush;
	if(debug){
		//lexer.rdata.lmod->dump();
		F->dump();
		cerr << flush;
	}
	//cout << "dumped" << endl << flush;
	void *FPtr = lexer.rdata.exec->getPointerToFunction(F);
	//cout << "ran" << endl << flush;
	if(n->returnType==functionClass){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		std::cout << "function<" << dat.getMyFunction() << ">" << endl << flush;
	}else if(n->returnType==classClass){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		ClassProto* cp = dat.getMyClass(lexer.rdata);
		std::cout <<  "class<" << cp << ", '"<< cp->name << "'>" << endl << flush;
	}else if(dat.getPointer()==NULL || type==VOIDTYPE || n->returnType==voidClass){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		std::cout << flush;
	}
	else if(n->returnType==doubleClass){
		double (*FP)() = (double (*)())(intptr_t)FPtr;
		auto t = FP();
		std::cout << t << endl << flush;
	} else if(n->returnType==intClass){
		int64_t (*FP)() = (int64_t (*)())(intptr_t)FPtr;
		int64_t t = FP();
		std::cout << t << endl << flush;
	} else if(n->returnType==byteClass){
		uint8_t (*FP)() = (uint8_t (*)())(intptr_t)FPtr;
		uint8_t t = FP();
		printf("%d\n",t);
		fflush(stdout);
	} else if(n->returnType==boolClass){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		auto t = FP();
		std::cout << (t?"true\n":"false\n") << flush;
	} else if(n->returnType==complexClass){
		auto (*FP)() = (complex (*)())(intptr_t)FPtr;
		complex t = FP();
		printc(t,true);
	} else if(n->returnType==charClass){
		auto (*FP)() = (char (*)())(intptr_t)FPtr;
		auto t = FP();
		printf("\'%c\'",t);
		std::cout << endl << flush;
	} else if(n->returnType==c_stringClass){
		auto (*FP)() = (char* (*)())(intptr_t)FPtr;
		auto t = FP();
		printf("\"%s\"",t);
		std::cout << endl << flush;
	}
	/*else if(n->returnType==stringClass){
		StringStruct (*FP)() = (StringStruct (*)())(intptr_t)FPtr;
		auto t = FP();
		String temp(t.data, t.length);
		std::cout << temp << endl << flush;
	} */
	else if(n->returnType->layoutType==PRIMITIVEPOINTER_LAYOUT || n->returnType->layoutType==POINTER_LAYOUT){
		void* (*FP)() = (void* (*)())(intptr_t)FPtr;
		auto t = FP();
		std::cout << n->returnType->name << "<" << t << ">" << endl << flush;
	} else{
		cerr << "Unknown temp type to JIT-evaluate " << n->returnType->name << endl << flush;
	}
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
	String file = "";
	String command = "";
	String output = "";
	bool llvmIR = false;
	bool interactive = false;
	bool forceInt = false;
	bool debug = false;
	for(int i = 1; i<argc; ++i){
		String s = String(argv[i]);
		if(startsWithEq(s, "--debug")){
			debug = testFor(s,"--debug");
		}
		else if(s=="-ir" || s=="--ir") { llvmIR=true; }
		else if(s=="-i") { forceInt = true; interactive = true; }
		else if(s=="-ni") { forceInt = true; interactive = false; }
		else if(startsWithEq(s, "--inter")){
			forceInt = true;
			interactive = testFor(s,"--inter");
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
	raw_fd_ostream* outStream;
	if(llvmIR){
		if(output=="-" || output==""){
			outStream = new raw_fd_ostream(1, true);
			output = "-";
		}
		else outStream = new raw_fd_ostream(output.c_str(), error);
		if(error.length()>0){
			cerr << error << endl << flush;
			exit(1);
		}
	}
	initClasses();

	if(interactive) {
		std::cout << "Optricks version 0.2.2" << endl << flush;
		std::cout << "Created by Billy Moses" << endl << endl << flush;
	}
	//TODO 2 x major decision
	//should ++ / -- be eliminated and replaced with +=1 and -=1
	// or should semicolons be strictly enforced
	// bad case print(3+3) ++ a
	// could be: print(3+3)++; a
	// or print(3+3); ++a
	// or print(3+3) + (+a);
	//could make function calls / indexing immune from post operators
	// but then object.attribute ++ a
	// OR could eliminated either post or pre

	//if semicolons were strictly enforced then
	//list comprehension could become an operator

	Lexer lexer(NULL,interactive?'\n':EOF);
	initializeBaseFunctions(lexer.rdata);
	initFuncsMeta(lexer.rdata);
	std::vector<String> files =
		{"./stdlib/stdlib.opt"};
	if(!interactive){
		if(command==""){
			files.push_back(files[0]);
			files[0]=file;
		}
		else{
			cerr << "Commands not supported yet!" << endl << flush;
			exit(1);
		}
		lexer.execFiles(false,files, outStream,debug,(output.length()==0)?1:((llvmIR)?2:3));
	}
	else{
		lexer.execFiles(true,files, outStream,debug,0);
		Statement* n;
		Stream* st = new Stream(file, true);
		lexer.f = st;
		std::cout << START << flush;
		/*
		st->force("4/2*3/4\n");
		st->force("extern double cos(double a); cos(3.14159)\n");
		st->force("lambda int a,int b: a+b\n");
		st->force("(lambda int a,int b: a+b)(4,5)\n");
		st->force("(lambda double a: (lambda double sq: sq*sq)(sin(a))+(lambda double sq: sq*sq)(cos(a)))(.9)\n");
		st->force("2+3.1\n");
		st->force("if true: putchar(71); else: putchar(72)\n");
		st->force("(lambda int a, int b, int c: if a<=b & a<=c: a; elif b<=a & b<=c: b; else: c)(1,2,3)\n");
		st->force("(lambda int a, int b, int c: if a<=b & a<=c putchar(a) elif b<=a & b<=c putchar(b) else putchar(c))(1,2,3)\n");
		st->force("if(true){ putchar(71); putchar(72); }\n");
		st->force("true?1:0\n");
		st->force("if(true){ int i = 74; putchar(i); i = 75; putchar(i); }\n");
		st->force("def int printr(int i){ int j = i+48; putchar(j); return i;}\n");
		st->force("def int printr(int i){ int j = i+48; putchar(j); return i;}; printr(9);\n");
		st->force("def int p2(int i){ if(i>0) p2(i-1); return putchar(i+48); }; printr(9);\n");
		st->force("def void hi(){ putchar(50); return; }");
		//*/
		//		st->force("if(true){ printr(9); def int printr(int i){ int j = i+48; putchar(j); return i;}; }\n"); //TODO allow
		//st->force("for(int i = 0; i<10; i= i+1) putchar(i+48)\n");
		//st->force("if(true){ for(int i = 0; i<10; i= i+1) putchar(i+48)} \n");
		//st->force("for(int i = 0; i<10; i+=1){ if(i==5) break; printi(i); }\n");
		//st->force("(def void (int i){for(int i = 0; i<10; i+=1){ if(i%2==0) continue; printi(i) } })(0);");
		//st->force("(def void () printi(72))()\n");
		//st->force("for(int i = 0; i<1000 i+=1) printd((def double (int i){ double a=1 auto b=a for(int j=3 j<=i j+=1){ auto tmp = a+b a = b b = tmp} return b})(i))\n");
		//st->force("for(int i = 0; i<1000 i+=1) printd((def auto (int i){ double a=1 auto b=a for(int j=3 j<=i j+=1){ auto tmp = a+b a = b b = tmp} return b})(i))\n");
		//st->force("\"hi\"\n"); debug = true;
		//st->force("(lambda complex a: a.real)(complex())\n");
		//st->force("(complex()).real\n");
		//st->force("(def complex (complex a){ a.real = 3; return a})(complex())\n");
		//st->force("def complex operator/(complex c, complex b){ return c; }\n");
		//st->force("complex()*complex()\n");
		//st->force("(lambda string s: s.length)('c')\n");
		//st->force("int i=7;\n");
		//st->force("(def void dgah(){print(hi); })()\n");
		//st->force("'hello'[0];\n");
		//st->force("for i in range(,10): print(i);\n");
		while(true){
			st->enableOut = true;
			st->trim(EOF);
			n = lexer.getNextStatement('\n',true);
			st->enableOut = false;
			bool first = true;
			while(n->getToken()!=T_VOID){
				first = false;
				execF(lexer,lexer.myMod, n,debug);
				st->done = false;
				if(st->last()=='\n' || st->peek()=='\n') break;
				bool reset = false;
				while(true){
					char c = st->peek();
					if(c==';') st->read();
					else if(isWhitespace(c)){
						if(c=='\n'){
							reset = true;
							break;
						}
						else st->read();
					} else break;
				} if(reset) break;
				st->done = false;
				st->enableOut = true;
				n = lexer.getNextStatement('\n',true);
				st->enableOut = false;
			}
			st->done = false;
			if(st->last()==EOF) break;
			std::cout << START << flush;
			while(st->peek()=='\n' || st->peek()==';') st->read();
			st->done = false;

			st->done = false;
			if(first) break;
		}
	}
	return 0;
}
