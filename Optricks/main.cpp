#define MAIN_CPP

#include "containers/settings.hpp"
#include <fstream>
#include <iostream>
/**
 * TODO resolution
 * a) register class names
 * b) register operator / function names (prototypes) with arguments
 * 		this includes class methods / constructors
 * c) register operator / function prototypes with default
 * 		this includes class methods / constructors
 * d) switch to opointers instead of late-resolves
 * e) type check everything
 */

template <class T>
ostream& operator<<(ostream&os, std::vector<T>& v)
{
		bool first = true;
		os<<"[";
		for(const auto& a:v){
			if(first){
				first= false;
				os<<a;
			}else os<<", "<<a;
		}
		return os<<"]";
}

#include "Lexer.hpp"

void execF(RData& r, Statement* n,bool debug){
	if(n==NULL) return;// NULL;
	if(debug && n->getToken()!=T_VOID) cout << n << endl << flush;
	n = n->simplify();
	n->resolvePointers();
	n->registerClasses(r);
	n->registerFunctionArgs(r);
	n->registerFunctionDefaultArgs();
	n->checkTypes();
	Type* type;
	type = n->returnType->type;
	if(type==NULL){
		cout << "Error null return type for class " + n->returnType->name ;
		type = VOIDTYPE;
	}
	FunctionType *FT = FunctionType::get(type, std::vector<Type*>(), false);
	Function *F = Function::Create(FT, Function::ExternalLinkage, "", r.lmod);//todo check this
	BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
	r.builder.SetInsertPoint(BB);
	Value* v = n->evaluate(r);
	if(debug)	v->dump();
	if(type!=VOIDTYPE)
		r.builder.CreateRet(v);
	else
		r.builder.CreateRetVoid();
	//cout << "testing cos" << cos(3) << endl << flush;
	verifyFunction(*F);
	//cout << "verified" << endl << flush;
	r.fpm->run(*F);
	//cout << "fpm" << endl << flush;
	if(debug){
		F->dump();
		cerr << flush;
	}
	//cout << "dumped" << endl << flush;
	void *FPtr = r.exec->getPointerToFunction(F);
	//cout << "ran" << endl << flush;
	if(n==NULL || type==VOIDTYPE){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		if(debug) cout <<  "Evaluated" << endl << flush;
	} else if(n->returnType==decClass){
		double (*FP)() = (double (*)())(intptr_t)FPtr;
		auto t = FP();
		if(debug) cout <<  "Evaluated to ";
		cout << t << endl << flush;
	} else if(n->returnType==intClass){
		uint64_t (*FP)() = (uint64_t (*)())(intptr_t)FPtr;
		auto t = FP();
		if(debug) cout <<  "Evaluated to ";
		printi(t, true);
	} else if(n->returnType==boolClass){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		auto t = FP();
		if(debug) cout <<  "Evaluated to ";
		cout << t << endl << flush;
	} else if(n->returnType==stringClass){
		StringStruct (*FP)() = (StringStruct (*)())(intptr_t)FPtr;
		auto t = FP();
		//cout << "String length of " << (int)(* (int*)(t.length)) << endl << flush;
		String temp(t.data, t.length);
		if(debug) cout <<  "Evaluated to ";
		//cout << temp << endl << flush;
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
	bool interactive = false;
	bool forceInt = false;
	bool debug = false;
	for(int i = 1; i<argc; ++i){
		String s = String(argv[i]);
		if(startsWithEq(s, "--debug")){
			debug = testFor(s,"--debug");
		}
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
	if(!forceInt) interactive = file=="";
	//ofstream fout (output);
	std::ofstream tmp(output, std::ofstream::out | std::ofstream::binary);
	ostream& outStream = (output=="-" || output=="")?cout:(tmp);
	initClasses();

	if(interactive) {
		cout << "Optricks version 0.1.6" << endl << flush;
		cout << "Created by Billy Moses" << endl << endl << flush;
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
	std::vector<String> files = {"./stdlib/stdlib.opt"};
	if(!interactive){
		files.push_back(file);
		lexer.execFiles(files, outStream,debug,output!="");
	}
	else{
		lexer.execFiles(files, outStream,debug,false);
		Statement* n;
		Stream* st = new Stream(file, true);
		lexer.f = st;
		cout << START << flush;
		/*
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
		while(true){
			st->enableOut = true;
			st->trim(EOF);
			n = lexer.getNextStatement('\n');
			st->enableOut = false;
			bool first = true;
			while(n->getToken()!=T_VOID){
				first = false;
				execF(lexer.rdata,n,debug);
				st->done = false;
				if(st->last()=='\n') break;
				while(st->peek()==';') st->read();
				st->done = false;
				st->enableOut = true;
				n = lexer.getNextStatement('\n');
				st->enableOut = false;
			}
			st->done = false;
			if(st->last()==EOF) break;
			cout << START << flush;
			while(st->peek()=='\n' || st->peek()==';') st->read();
			st->done = false;

			st->done = false;
			if(first) break;
		}
	}
	return 0;
}
