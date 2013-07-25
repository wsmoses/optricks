
#define MAIN_CPP
#include "O_IO.hpp"
#include "O_Stream.hpp"
#include "O_Token.hpp"
#include "O_Expression.hpp"

std::vector<String> getCommaSeparated(Stream* f, char endWith){
	std::vector<String> vals;
	do{
		if(f->trim(endWith)) return vals;
		String temp = f->getNextName(endWith);
		if(temp.size()==0) return vals;
		vals.push_back(temp);
		if(f->trim(endWith)) return vals;
		if(f->done || f->peek()!=',') return vals;
		else f->read();
	}
	while(true);
	return vals;
}
std::vector<DefaultDeclaration*> parseArguments(Stream* f, int endWith){
	std::vector<DefaultDeclaration*> args;
	if(f->done || f->peek()!='(') return args;
	f->read();
	while(true){

		if(f->trim(endWith)) return args;
		if(f->peek()==')'){
			f->read();
			return args;
		}
		String type = f->getNextName(endWith);

		if(f->trim(endWith)){
			if(type.size()>0)
				args.push_back(new DefaultDeclaration(new E_VAR(type)));
			return args;
		}

		String name = f->getNextName(endWith);

		if(name.length()==0){
			name = type;
			type = "";
		}

		if(f->trim(endWith)){
			args.push_back(new DefaultDeclaration(new E_VAR(name), (type.length()>0)?(new E_VAR(type)):NULL));
			return args;
		}

		Expression* defVal = NULL;

		if(!f->done && f->peek()=='='){
			f->read();
			defVal = getNextExpression(f, endWith);
			if(defVal->getToken()==T_EOF) f->error("Could not parse default arg");
		}


		args.push_back(new DefaultDeclaration(new E_VAR(name), (type.length()>0)?(new E_VAR(type)):NULL, defVal));
		char tchar = f->peek();
		if(tchar==')' || tchar==',' || tchar==';'){
			f->read();
			if(tchar==')') return args;
		}
		else f->error("Could not parse arguments");
	}
	return args;
}


Expression* getNextExpression(Stream* f, char endWith, bool opCheck) {
	if(f->done || f->trim(endWith)) return new E_EOF();
	int nex = f->peek();
	if(f->done || nex==EOF || nex==endWith) return new E_EOF();
	Expression* toReturn = NULL;
	if (isalpha(nex) || nex=='_' || nex=='$') {
		String temp = f->getNextName(endWith);
		if (temp == "def" || temp=="function" || temp=="method" ){
			if(f->trim(endWith)) f->error("Uncompleted function");
			String methodName = f->getNextName(endWith);
			if(f->trim(endWith)) f->error("Uncompleted function (with name)");
			std::vector<DefaultDeclaration*> arguments;
			if(!f->done && f->peek()=='('){
				arguments = parseArguments(f, endWith);
			}
			if(f->trim(endWith)) f->error("Function without body");
			if(!f->done && f->peek()==':'){
				f->read();
				if(f->trim(endWith)) f->error("Function without body (c)");
			}
			Expression* methodBody;
			if(!f->done && f->peek()=='{'){
				f->read();
				E_ARR* data = new E_ARR();
				while(true){
					if(f->done || f->peek()=='}') break;
					Expression* t = getNextExpression(f,endWith);
					if(t->getToken()==T_EOF) break;
					data->values.push_back(t);
				}
				if(f->read()!='}') f->error("No } found at end",true);
				methodBody = data;
			}
			else{
				//TODO check if adding false is important
				methodBody = getNextExpression(f,endWith, false);
			}

			toReturn = new OBJ_FUNC(methodName, arguments, methodBody);

			if(opCheck)
				toReturn = operatorCheck(f, toReturn, endWith);
		}
		else if (temp == "extern"){
			if(f->trim(endWith)) f->error("Extern without name");
			std::vector<String> vals = getCommaSeparated(f, endWith);

			if(!f->done && f->peek()==';') f->read();
			if(vals.size()>1){
				E_ARR* arr = new E_ARR();
				for (auto &a: vals){
					arr->values.push_back((new E_EXT(a)));
				}
				toReturn = arr;
			}
			else if (vals.size()>0){
				toReturn = new E_EXT(vals[0]);
			}
			//toReturn->write(cout) << endl;
			//if(opCheck)
			//toReturn = operatorCheck(f, toReturn,endWith);
		}
		else{
			toReturn =  new E_VAR(temp);
			if(opCheck)
				toReturn = operatorCheck(f,toReturn,endWith);
		}
	}
	else{
		Expression* temp;
		char te;
		bool forceAr;
		switch(nex){
			case '.':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				toReturn = f->readNumber(endWith);
				if(opCheck)	toReturn = operatorCheck(f, toReturn,endWith);
				break;
			case '\'':
			case '"':
				toReturn = new OBJ_STR(f->readString(nex));
				if(opCheck)	toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '{':
				//TODO
				f->read();
				f->trim(endWith);
				toReturn = new E_ARR();
				if(f->peek()=='}') f->read();
				else{
					temp = getNextExpression(f,endWith);
					while(temp->getToken()!=T_EOF){
						((E_ARR*)toReturn)->values.push_back(temp);
						bool comma = true;
						if(!f->done && f->peek()==',') f->read();
						else comma = false;
						if(f->trim(endWith)) f->error("Uncompleted '{' array",true);
						if(!comma){
							if(f->peek()=='}') break;
							else f->error("Missing , in '{' array or wrong end char",true);
						}
						temp = getNextExpression(f,endWith);
					}
					if(f->done) f->error("Uncompleted '{' array 2");
					if((te = f->read())!='}') f->error("Cannot end '{' array with "+te,true);
				}
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '[':
				//TODO
				f->read();
				f->trim(endWith);
				toReturn = new E_ARR();
				if(f->peek()==']') f->read();
				else{
					temp = getNextExpression(f,endWith);
					while(temp->getToken()!=T_EOF){
						((E_ARR*)toReturn)->values.push_back(temp);
						bool comma = true;
						if(!f->done && f->peek()==',') f->read();
						else comma = false;
						if(f->trim(endWith)) f->error("Uncompleted '[' array",true);
						if(!comma){
							if(f->peek()==']') break;
							else f->error("Missing , in '[' array or wrong end char",true);
						}
						temp = getNextExpression(f,endWith);
					}
					if(f->done)	f->error("Uncompleted '[' array 2",true);
					if((te = f->read())!=']') f->error("Cannot end '[' array with "+te,true);
				}
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '(':
				f->read();
				f->trim(endWith);
				toReturn = new E_ARR();
				if(f->peek()==')') f->read();
				else{
					temp = getNextExpression(f,endWith);
					forceAr = false;
					while(temp->getToken()!=T_EOF){
						((OBJ_ARR*)toReturn)->values.push_back(temp);
						bool comma = true;
						if(!f->done && f->peek()==','){
							f->read();
							forceAr = true;
						}
						else comma = false;
						if(f->trim(endWith)) f->error("Uncompleted '(' arrayt",true);

						if(f->peek()==')') break;
						if(!comma){
							f->error("Missing , in '(' array or wrong end char",true);
						}
						if(f->trim(endWith)) f->error("Uncompleted '(' array",true);
						temp = getNextExpression(f,endWith);
					}
					if(!forceAr && ((OBJ_ARR*)toReturn)->values.size()==1){
						Expression* temp = ((OBJ_ARR*)toReturn)->values[0];
						free(toReturn);
						toReturn = new E_PARENS(temp);
					}
					if(f->done) f->error("Uncompleted '(' 2");
					if((te = f->read())!=')') f->error("Cannot end '(' array with "+te);
				}
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '+':
				f->read();
				toReturn =  new UO_POS(getNextExpression(f, endWith));
				if(opCheck) toReturn = operatorCheck(f,toReturn, endWith);
				break;
			case '-':
				f->read();
				toReturn =  new UO_NEG(getNextExpression(f, endWith));
				if(opCheck)
					toReturn = operatorCheck(f,toReturn, endWith);
				break;
			case '~':
				f->read();
				toReturn = new UO_BNOT(getNextExpression(f, endWith));
				if(opCheck)
					toReturn = operatorCheck(f,toReturn, endWith);
				break;
			case '!':
				f->read();
				toReturn = new UO_NOT(getNextExpression(f, endWith));
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '*':
				f->read();
				toReturn = new UO_DEREF(getNextExpression(f, endWith));
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
			case '&':
				f->read();
				toReturn = new UO_REF(getNextExpression(f, endWith));
				if(opCheck)
					toReturn = operatorCheck(f, toReturn, endWith);
				break;
				//		case '@':
		}
	}
	if(toReturn==NULL){
		f->error("Unknown rest of file");
		fprintf(stderr, "$$ %d %c\n", nex, nex);
		fflush(stderr);
		return new E_EOF();
	}
	else{
		if(!f->done && f->peek()==';') f->read();
		f->trim(endWith);
		return toReturn;

	}
}

/*
//===----------------------------------------------------------------------===//
// Abstract Syntax Tree (aka Parse Tree)
//===----------------------------------------------------------------------===//

/// ExprAST - Base class for all expression nodes.
class ExprAST {
	public:
		virtual ~ExprAST() {}
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
		double Val;
	public:
		NumberExprAST(double val) : Val(val) {}
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
		String Name;
	public:
		VariableExprAST(const String &name) : Name(name) {}
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
		char Op;
		ExprAST *LHS, *RHS;
	public:
		BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
	: Op(op), LHS(lhs), RHS(rhs) {}
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
		String Callee;
		std::vector<ExprAST*> Args;
	public:
		CallExprAST(const String &callee, std::vector<ExprAST*> &args)
	: Callee(callee), Args(args) {}
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
		String Name;
		std::vector<String> Args;
	public:
		PrototypeAST(const String &name, const std::vector<String> &args)
	: Name(name), Args(args) {}

};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
		PrototypeAST *Proto;
		ExprAST *Body;
	public:
		FunctionAST(PrototypeAST *proto, ExprAST *body)
	: Proto(proto), Body(body) {}

};


/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
static std::map<char, int> BinopPrecedence;

/// GetTokPrecedence - Get the precedence of the pending binary operator token.
static int GetTokPrecedence(char token) {
	if (!isascii(token))
		return -1;

	// Make sure it's a declared binop.
	int TokPrec = BinopPrecedence[token];
	if (TokPrec <= 0) return -1;
	return TokPrec;
}

/// Error* - These are little helper functions for error handling.
ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

static ExprAST *ParseExpression();

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
static ExprAST* ParseIdentifierExpr(FILE* f) {
	String IdName;
	getNextToken(f, &IdName);

	if (fpeek(f) != '(') // Simple variable ref.
		return new VariableExprAST(IdName);

	// Call.
	getNextToken();  // eat (
	std::vector<ExprAST*> Args;
	if (CurTok != ')') {
		while (1) {
			ExprAST *Arg = ParseExpression();
			if (!Arg) return 0;
			Args.push_back(Arg);

			if (CurTok == ')') break;

			if (CurTok != ',')
				return Error("Expected ')' or ',' in argument list");
			getNextToken();
		}
	}

	// Eat the ')'.
	getNextToken();

	return new CallExprAST(IdName, Args);
}


/// parenexpr ::= '(' expression ')'
static ExprAST *ParseParenExpr() {
	getNextToken();  // eat (.
	ExprAST *V = ParseExpression();
	if (!V) return 0;

	if (CurTok != ')')
		return Error("expected ')'");
	getNextToken();  // eat ).
	return V;
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static ExprAST *ParsePrimary() {
	switch (CurTok) {
		default: return Error("unknown token when expecting an expression");
		case tok_identifier: return ParseIdentifierExpr();
		case tok_number:     return ParseNumberExpr();
		case '(':            return ParseParenExpr();
	}
}

/// binoprhs
///   ::= ('+' primary)*
static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
	// If this is a binop, find its precedence.
	while (1) {
		int TokPrec = GetTokPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;

		// Okay, we know this is a binop.
		int BinOp = CurTok;
		getNextToken();  // eat binop

		// Parse the primary expression after the binary operator.
		ExprAST *RHS = ParsePrimary();
		if (!RHS) return 0;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = GetTokPrecedence();
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec+1, RHS);
			if (RHS == 0) return 0;
		}

		// Merge LHS/RHS.
		LHS = new BinaryExprAST(BinOp, LHS, RHS);
	}
}

/// expression
///   ::= primary binoprhs
///
static ExprAST *ParseExpression() {
	ExprAST *LHS = ParsePrimary();
	if (!LHS) return 0;

	return ParseBinOpRHS(0, LHS);
}

/// prototype
///   ::= id '(' id* ')'
static PrototypeAST *ParsePrototype() {
	if (CurTok != tok_identifier)
		return ErrorP("Expected function name in prototype");

	String FnName = IdentifierStr;
	getNextToken();

	if (CurTok != '(')
		return ErrorP("Expected '(' in prototype");

	std::vector<String> ArgNames;
	while (getNextToken() == tok_identifier)
		ArgNames.push_back(IdentifierStr);
	if (CurTok != ')')
		return ErrorP("Expected ')' in prototype");

	// success.
	getNextToken();  // eat ')'.

	return new PrototypeAST(FnName, ArgNames);
}

/// definition ::= 'def' prototype expression
static FunctionAST *ParseDefinition() {
	getNextToken();  // eat def.
	PrototypeAST *Proto = ParsePrototype();
	if (Proto == 0) return 0;

	if (ExprAST *E = ParseExpression())
		return new FunctionAST(Proto, E);
	return 0;
}

/// toplevelexpr ::= expression
static FunctionAST *ParseTopLevelExpr() {
	if (ExprAST *E = ParseExpression()) {
		// Make an anonymous proto.
		PrototypeAST *Proto = new PrototypeAST("", std::vector<String>());
		return new FunctionAST(Proto, E);
	}
	return 0;
}

/// external ::= 'extern' prototype
static PrototypeAST *ParseExtern() {
	getNextToken();  // eat extern.
	return ParsePrototype();
}

//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//

static void HandleDefinition() {
	if (ParseDefinition()) {
		fprintf(stderr, "Parsed a function definition.\n");
	} else {
		// Skip token for error recovery.
		getNextToken();
	}
}

static void HandleExtern() {
	if (ParseExtern()) {
		fprintf(stderr, "Parsed an extern\n");
	} else {
		// Skip token for error recovery.
		getNextToken();
	}
}

static void HandleTopLevelExpression() {
	// Evaluate a top-level expression into an anonymous function.
	if (ParseTopLevelExpr()) {
		fprintf(stderr, "Parsed a top-level expr\n");
	} else {
		// Skip token for error recovery.
		getNextToken();
	}
}

/// top ::= definition | external | expression | ';'
static void MainLoop() {
	while (1) {
		fprintf(stderr, "ready> ");
		switch (CurTok) {
			case t_EOF:    return;
			case ';':        getNextToken(); break;  // ignore top-level semicolons.
			case tok_def:    HandleDefinition(); break;
			case tok_extern: HandleExtern(); break;
			default:         HandleTopLevelExpression(); break;
		}
	}
}
 */
int main() {
	cout << "EOF = " << (int)EOF << endl;
	Stream* st = new Stream(stdin,true);
	Expression* n;
	cout << "ready> " << flush;
	while(true){
		st->trim(EOF);
		n = getNextExpression(st,'\n');
		bool first = true;
		while(n->getToken()!=T_EOF){
			first = false;
			cout << "Read-exp: " << n << endl;
			n = getNextExpression(st, '\n');
		}
		st->done = false;
		cout << "ready> " << flush;
		while(st->peek()=='\n') st->read();

		st->done = false;
		if(first) break;
	}
	return 0;
}
