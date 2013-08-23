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

void execF(RData& r, Statement* n,bool interactive){
	if(n==NULL) return;// NULL;
	n = n->simplify();
	if(interactive) cout << n << endl << flush;
	n->registerClasses(r);
	n->registerFunctionArgs(r);
	n->registerFunctionDefaultArgs();
	n->resolvePointers();
	n->checkTypes();
	Type* type;
	type = n->returnType->type;
	if(type==NULL){
		if(interactive) cout << "Error null return type for class " + n->returnType->name ;
		type = VOIDTYPE;
	}
	FunctionType *FT = FunctionType::get(type, std::vector<Type*>(), false);
	Function *F = Function::Create(FT, Function::ExternalLinkage, "", r.lmod);//todo check this
	BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
	r.builder.SetInsertPoint(BB);
	Value* v = n->evaluate(r);
	if(interactive)	v->dump();
	if(type!=VOIDTYPE)
		r.builder.CreateRet(v);
	else
		r.builder.CreateRetVoid();
	//cout << "testing cos" << cos(3) << endl << flush;
	verifyFunction(*F);
	//cout << "verified" << endl << flush;
	r.fpm->run(*F);
	//cout << "fpm" << endl << flush;
	if(interactive){
		F->dump();
		cerr << flush;
	}
	//cout << "dumped" << endl << flush;
	void *FPtr = r.exec->getPointerToFunction(F);
	//cout << "ran" << endl << flush;
	if(n==NULL || type==VOIDTYPE){
		void (*FP)() = (void (*)())(intptr_t)FPtr;
		FP();
		if(interactive) cout <<  "Evaluated" << endl << flush;
	} else if(n->returnType==decClass){
		double (*FP)() = (double (*)())(intptr_t)FPtr;
		auto t = FP();
		if(interactive) cout <<  "Evaluated to " << t << endl << flush;
	} else if(n->returnType==intClass){
		long long (*FP)() = (long long (*)())(intptr_t)FPtr;
		auto t = FP();
		if(interactive) cout <<  "Evaluated to " << t << endl << flush;
	} else if(n->returnType==boolClass){
		bool (*FP)() = (bool (*)())(intptr_t)FPtr;
		auto t = FP();
		if(interactive) cout <<  "Evaluated to " << t << endl << flush;
	} else{
		cerr << "Unknown temp type to JIT-evaluate " << n->returnType->name << endl << flush;
	}
}
int main(int argc, char** argv){

	bool interactive = argc<2;
	initClasses();

	if(interactive) {
		cout << "Optricks version 0.1.3" << endl << flush;
		cout << "Created by Billy Moses" << endl << flush;
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

	Stream* st = new Stream(stdin,interactive);
	Statement* n;
	if(interactive)	cout << "ready> " << flush;
	//st->force("extern double cos(double a); cos(3.14159)\n");
	//st->force("lambda int a,int b: a+b\n");
	//st->force("(lambda int a,int b: a+b)(4,5)\n");
	//st->force("(lambda double a: (lambda double sq: sq*sq)(sin(a))+(lambda double sq: sq*sq)(cos(a)))(.9)\n");
	//st->force("2+3.1\n");
	Lexer lexer(st,interactive?'\n':EOF);
	lexer.execFile("./stdlib/stdlib.opt",true, true);
	while(true){
		st->trim(EOF);
		n = lexer.getNextStatement();
		bool first = true;
		while(n->getToken()!=T_VOID){
			first = false;
			execF(lexer.rdata,n,interactive);
			st->done = false;
			if(st->last()==EOF) break;
			while(st->peek()==';') st->read();
			st->done = false;
			n = lexer.getNextStatement();
		}
		st->done = false;
		if(st->last()==EOF) break;
		while(st->peek()=='\n' || st->peek()==';') st->read();
		st->done = false;
		if(interactive)
			cout << "ready> " << flush;

		st->done = false;
		if(first) break;
	}
	return 0;
}
