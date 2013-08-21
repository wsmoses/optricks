#define MAIN_CPP

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

void execF(RData& r, Statement* n){
	if(n==NULL) return;// NULL;
	Jump temp = NJUMP;
	n = n->simplify(temp);
	cout << n << endl << flush;
	n->checkTypes();
	Type* type;
	Expression* e = dynamic_cast<Expression*>(n);
	if(e!=NULL){
		type = e->returnType->type;
		if(type==NULL){
			cout << "Error null return type for class " + e->returnType->name ;
			type = VOIDTYPE;
		}
	}
	else type = VOIDTYPE;
		FunctionType *FT = FunctionType::get(type, std::vector<Type*>(), false);
		Function *F = Function::Create(FT, Function::ExternalLinkage, "", r.lmod);//todo check this
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		r.builder.SetInsertPoint(BB);
		Value* v = n->evaluate(r);
		v->dump();
		if(type!=VOIDTYPE)
			r.builder.CreateRet(v);
		else
			r.builder.CreateRetVoid();
		//cout << "testing cos" << cos(3) << endl << flush;
		verifyFunction(*F);
		//cout << "verified" << endl << flush;
		r.fpm->run(*F);
		//cout << "fpm" << endl << flush;
		F->dump();
		cerr << flush;
		//cout << "dumped" << endl << flush;
		void *FPtr = r.exec->getPointerToFunction(F);
		//cout << "ran" << endl << flush;
		if(e==NULL || type==VOIDTYPE){
			void (*FP)() = (void (*)())(intptr_t)FPtr;
			FP();
			cout <<  "Evaluated" << endl << flush;
		} else if(e->returnType==decClass){
			double (*FP)() = (double (*)())(intptr_t)FPtr;
			cout <<  "Evaluated to " << FP() << endl << flush;
		} else if(e->returnType==intClass){
			long long (*FP)() = (long long (*)())(intptr_t)FPtr;
			cout <<  "Evaluated to " << FP() << endl << flush;
		} else if(e->returnType==boolClass){
			bool (*FP)() = (bool (*)())(intptr_t)FPtr;
			cout <<  "Evaluated to " << FP() << endl << flush;
		} else{
			cerr << "Unknown temp type to JIT-evaluate " << e->returnType->name << endl << flush;
		}
}
int main(int argc, char** argv){
	cout << "Optricks version 0.2.1" << endl << flush;
	initClasses();
	cout << "Created by Billy Moses" << endl << flush;
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

	bool interactive = argc<2;
	Stream* st = new Stream(stdin,interactive);
	Lexer lexer(st,interactive?'\n':EOF);
	Statement* n;
	if(interactive)	cout << "ready> " << flush;
	st->force("extern double cos(double a); cos(3.14159/2)\n");
	//st->force("2+3.1\n");
	OModule* m = new OModule(LANG_M);
	while(true){
		st->trim(EOF);
		n = lexer.getNextStatement(m, true, true);
		bool first = true;
		while(n->getToken()!=T_VOID){
			first = false;
			execF(lexer.rdata,n);
			n = lexer.getNextStatement(m, true, true);
		}
		st->done = false;
		if(st->last()==EOF) break;
		if(interactive)
			cout << "ready> " << flush;
		while(st->peek()=='\n' || st->peek()==';') st->read();

		st->done = false;
		if(first) break;
	}
	return 0;
}
