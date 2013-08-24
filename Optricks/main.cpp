#define MAIN_CPP

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
#include "containers/settings.hpp"
#define START "optricks> "
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
	n = n->simplify();
	if(debug) cout << n << endl << flush;
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
		long long (*FP)() = (long long (*)())(intptr_t)FPtr;
		auto t = FP();
		if(debug) cout <<  "Evaluated to ";
		cout << t << endl << flush;
	} else if(n->returnType==boolClass){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		auto t = FP();
		if(debug) cout <<  "Evaluated to ";
		cout << t << endl << flush;
	} else{
		cerr << "Unknown temp type to JIT-evaluate " << n->returnType->name << endl << flush;
	}
}
int main(int argc, char** argv){

	bool interactive = argc<2;
	initClasses();

	if(interactive) {
		cout << "Optricks version 0.1.5" << endl << flush;
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

	bool debug = false;
	Lexer lexer(NULL,interactive?'\n':EOF);
	lexer.execFile("./stdlib/stdlib.opt",true, true);
	if(!interactive)
		lexer.execFile("stdin",false, false,stdin);
	else{

		Statement* n;
		Stream* st = new Stream(stdin,interactive);
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
		while(true){
			st->trim(EOF);
			n = lexer.getNextStatement();
			bool first = true;
			while(n->getToken()!=T_VOID){
				first = false;
				execF(lexer.rdata,n,debug);
				st->done = false;
				if(st->last()=='\n') break;
				while(st->peek()==';') st->read();
				st->done = false;
				n = lexer.getNextStatement();
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
