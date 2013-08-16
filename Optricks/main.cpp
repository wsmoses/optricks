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
	if(interactive)
		cout << "ready> " << flush;
	while(true){
		st->trim(EOF);
		n = lexer.getNextStatement(true, true);
		bool first = true;
		while(n->getToken()!=T_VOID){
			first = false;
			cout << n << endl << flush;
			Jump temp = NJUMP;
			cout << n->simplify(temp) << endl << flush;
			temp = NJUMP;
			if(n!=NULL){
				n->checkTypes();
				Value* v = (n->evaluate(lexer.rdata,getGlobalContext()));
				v->dump();
				cerr << endl << flush;
				cout << flush;
			//cout << " " <<v << endl << flush;
			}
			n = lexer.getNextStatement(true, true);
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
