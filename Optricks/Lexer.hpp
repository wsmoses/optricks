/*
 * Lexer.hpp
 *
 *  Created on: Aug 6, 2013
 *      Author: wmoses
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include "containers/settings.hpp"

#include "O_Stream.hpp"
#include "constructs/Declaration.hpp"
#include "constructs/WhileLoop.hpp"
#include "constructs/ForLoop.hpp"
#include "constructs/ForEachLoop.hpp"
#include "constructs/IfStatement.hpp"
#include "constructs/Block.hpp"
#include "expressions/E_RETURN.hpp"
#include "expressions/E_BINOP.hpp"
#include "expressions/E_UOP.hpp"
#include "expressions/E_VAR.hpp"
#include "expressions/E_SET.hpp"
#include "expressions/E_TERNARY.hpp"
#include "primitives/oobject.hpp"
#include "primitives/obool.hpp"

class Lexer{
	public:
		Stream* f;
		OModule* myMod;
		//char endWith;
		//	bool inter;
		RData rdata;
		Lexer(Stream* t, bool inter):f(t),rdata(){
			//endWith=EOF;
			myMod = new OModule(LANG_M);
		}
		void execFile(String fileName, bool newModa, bool newModb, bool debug){
			Stream* tmp = f;
			Stream next(fileName,false);
			f = &next;
			if(newModa) myMod = new OModule(myMod);
			std::vector<Statement*> stats;
			while(true){
				while(f->peek()==';') f->read();
				Statement* s = getNextStatement(EOF);
				if(s==NULL || s->getToken()==T_VOID) break;
				if(debug && s->getToken()!=T_VOID){
					cout << s << ";" << endl << endl << flush;
				}
				s = s->simplify();
				stats.push_back(s);
			}
			for(auto& n: stats) n->resolvePointers();
			for(auto& n: stats) n->registerClasses(rdata);
			for(auto& n: stats) n->registerFunctionArgs(rdata);
			for(auto& n: stats) n->registerFunctionDefaultArgs();
			for(auto& n: stats) n->checkTypes();
			FunctionType *FT = FunctionType::get(VOIDTYPE, std::vector<Type*>(), false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, "", rdata.lmod);//todo check this
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
			rdata.builder.SetInsertPoint(BB);
			for(auto& n: stats){
				auto t = n->evaluate(rdata);
			}
			if(debug){
				rdata.lmod->dump();
				cerr << endl << flush;
			}
			rdata.builder.CreateRetVoid();
			verifyFunction(*F);
			rdata.fpm->run(*F);
			void *FPtr = rdata.exec->getPointerToFunction(F);
			void (*FP)() = (void (*)())(intptr_t)FPtr;
			FP();
			f->close();
			f = tmp;
			if(newModb) myMod = new OModule(myMod);
		}
		String getNextName(char endWith){
			f->trim(endWith);
			auto temp = f->getNextName(endWith);
			if(temp.length()==0) f->error("Variable Name of Length 0, next char is "+String(1,f->peek()));
			if(in<String>(RESERVED_KEYWORDS, temp)) f->error("Variable name is a reserved keyword");
			if(in<String>(BINARY_OPERATORS, temp)) f->error("Variable name is a binary operator");
			return temp;
		}
		E_VAR* getNextVariable(char endWith, OModule* mod, bool late=true){
			Resolvable* pointer;
			if(late) pointer = new LateResolve(mod, getNextName(endWith), pos());
			else pointer = mod->addPointer(pos(), getNextName(endWith),NULL,NULL,NULL,NULL,NULL);
			return new E_VAR(pos(), pointer);
		}
		Declaration* getNextDeclaration(char endWith, OModule* mod){
			f->trim(endWith);
			Statement* declarationType = getNextType(EOF, mod);
			f->trim(EOF);
			String varName = getNextName(endWith);
			f->trim(endWith);
			Statement* value = NULL;
			if(f->peek()=='='){
				f->read();
				value = getNextStatement(EOF, mod, true,false);
			}
			E_VAR* variable = new E_VAR(pos(), mod->addPointer(pos(), varName,NULL,NULL,NULL,NULL,NULL)); // TODO look at
			return new Declaration(pos(), declarationType, variable, value);
		}
		Statement* getNextStatement(char endWith){
			return getNextStatement(endWith, myMod, true, true);
		}
		std::vector<Declaration*> parseArguments(char endWith, OModule* m, char finish=')'){
			std::vector<Declaration*> args;
			if(f->done) return args;
			while(true){
				if(f->trim(endWith)) return args;
				if(f->peek()==finish){
					f->read();
					return args;
				}
				Declaration* d = getNextDeclaration(EOF, m);
				for(auto& a:args){
					if(d->variable->pointer->name == a->variable->pointer->name){
						f->error("Cannot have duplicate argument name: "+a->variable->pointer->name, true);
					}
				}
				args.push_back(d);
				f->trim(endWith);
				char tchar = f->peek();
				if(tchar==finish || tchar==','){
					f->read();
					if(tchar==finish) return args;
				}
				else f->error("Could not parse arguments - encountered character "+String(1,tchar));
			}
			return args;
		}
		PositionID pos(){
			return f->pos();
		}
		Statement* getNextBlock(char endWith, OModule* m, bool*par=NULL){
			OModule* module = new OModule(m);
			f->trim(EOF);
			bool paren = f->peek()=='{';
			if(paren) f->read();
			if(par!=NULL) *par=paren;
			if(paren){
				Block* blocks = new Block(pos());
				f->trim(EOF);
				while(f->peek()!='}'){
					Statement* e = getNextStatement(EOF, module, true, true);
					if(e!=NULL && e->getToken()!=T_VOID) blocks->values.push_back(e);
					f->trim(EOF);
					while(!f->done && f->peek()==';'){f->read();f->trim(endWith);}
					f->trim(EOF);
				}
				f->trim(endWith);
				if(paren && f->read()!='}') f->error("Need '}' for ending block statement");
				return blocks;
			}
			else{
				Statement* s = getNextStatement(endWith, module, true, true);//todo check
				while(!f->done && f->peek()==';'){f->read();f->trim(endWith);}
				f->trim(endWith);
				return s;
			}
		}
		Statement* getNextType(char endWith, OModule* mod){
			f->trim(EOF);
			if(f->done || !isStartName(f->peek())) f->error("Could not find alphanumeric start for type parsing, found "+String(1,f->peek()));
			Statement* currentType = getNextVariable(endWith, mod);
			do{
				f->trim(endWith);
				auto marker = f->getMarker();
				String typeOperation = f->getNextOperator(endWith);
				if(typeOperation=="" || !in<String>(TYPE_OPERATORS, typeOperation)){
					f->undoMarker(marker);
					break;
				}
				f->trim(endWith);
				if(f->done || isStartName(f->peek())) f->error("Could not find alphanumeric start for type parsing");
				auto nextVariable = getNextName(endWith);
				currentType = new E_LOOKUP(pos(), currentType, nextVariable, typeOperation);
			}while(true);
			return currentType;
		}
		Statement* getNextStatement(char endWith, OModule* mod, bool opCheck, bool allowDeclaration){
			if(f->done || f->trim(EOF)) return VOID;
			int nex = f->peek();
			if(f->done || nex==EOF /*|| nex==endWith*/) return VOID;
			if(nex==';') return VOID;
			bool semi;
			if (isStartName(nex)) {
				auto undoRead = f->getMarker();
				String temp = f->getNextName(endWith);
				if(temp=="if"){
					if(f->trim(EOF)) f->error("Uncompleted if");
					std::vector<std::pair<Statement*,Statement* >> statements;
					Statement* c = getNextStatement(EOF, mod, true,false);
					if(c->getToken()==T_VOID) f->error("Need condition for if");
					if(!f->done && f->peek()==':') f->read();
					Statement* s = getNextBlock(endWith, mod);
					if(c->getToken()==T_VOID) f->error("Need expression for if");
					statements.push_back(std::pair<Statement*,Statement* >(c,s));
					f->trim(endWith);
					while(!f->done && f->peek()==';') f->read();
					auto marker = f->getMarker();
					String test = f->getNextName(endWith);
					Statement* finalElse = VOID;
					while(!f->done && (test=="else" || test=="elif")){
						f->trim(EOF);
						bool elif = test=="elif";
						if(!elif){
							auto m = f->getMarker();
							String yy = f->getNextName(endWith);
							if(yy=="if"){ elif=true; f->trim(EOF); }
							else { f->undoMarker(m); }
						}
						if(elif){
							c = getNextStatement(EOF, mod, true,false);
							if(!f->done && f->peek()==':') f->read();
							s = getNextBlock(endWith, mod);
							statements.push_back(std::pair<Statement*,Statement* >(c,s));
							f->trim(endWith);
							while(!f->done && f->peek()==';') f->read();
						}
						else{
							if(!f->done && f->peek()==':') f->read();
							f->trim(EOF);
							finalElse = getNextBlock(endWith, mod);
							f->trim(endWith);
							while(!f->done && f->peek()==';') f->read();
						}
						marker = f->getMarker();
						test = f->getNextName(endWith);
					}
					f->undoMarker(marker);
					Statement* building = finalElse;
					for(unsigned int i = statements.size()-1; ; i--){
						building = new IfStatement(pos(), statements[i].first, statements[i].second, building);
						if(i==0) break;
					}
					return building;
				}
				else if(temp=="for"){
					if(f->trim(EOF)) f->error("Uncompleted for",true);
					bool paren = f->peek()=='(';
					//Standard for(i = 0; i<7; i++)
					OModule* module = new OModule(mod);
					if(paren){
						f->read();
						f->trim(EOF);
						Statement* init = VOID;
						if(f->peek()!=';') init = getNextStatement(EOF, module, true, true);
						if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
						f->trim(EOF);
						Statement* cond = VOID;
						if(f->peek()!=';') cond = getNextStatement(EOF, module, true, false);
						if(cond->getToken()==T_VOID) cond = new obool(pos(), true);
						if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
						f->trim(EOF);
						Statement* inc = VOID;
						if(f->peek()!=')') inc = getNextStatement(EOF, module, true, false);
						f->trim(EOF);
						if(f->read()!=')') f->error("Invalid additional piece of for loop",true);
						Statement* blocks = getNextBlock(endWith, module);
						return new ForLoop(pos(), init,cond,inc,blocks);
						//TODO implement for loop naming
					}
					else{
						String iterName = getNextName(EOF);
						f->trim(EOF);
						bool as = f->getNextName(endWith)=="in";
						f->trim(EOF);
						bool col = false;
						if(!f->done && f->peek()==':'){
							f->read(); f->trim(EOF);
						}
						if(as==col){
							f->error("Need either ':' or 'in' to separate iterator variable from iterable");
						}
						Statement* iterable = getNextStatement(EOF, mod, true,false);
						f->trim(EOF);
						if(!f->done && f->peek()==':'){
							f->read();
							f->trim(EOF);
						}
						Statement* blocks = getNextBlock(endWith, module);
						f->error("Implement for-each loop");
						//return new ForEachLoop(new E_VAR(module->addPointer(varName,NULL,NULL,NULL)),iterable,blocks,"");
						//TODO implement for loop naming
					}
				}
				else if(temp=="while"){
					if(f->trim(EOF)) f->error("Uncompleted while",true);
					bool paren = f->peek()=='(';
					if(paren) f->read();
					Statement* cond = getNextStatement(EOF, mod, true,false);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of while",true);
					if(f->trim(EOF)) f->error("Uncompleted while",true);
					if(f->peek()==':') f->read();
					return new IfStatement(pos(), cond, new DoWhileLoop(pos(), cond,getNextBlock(endWith, mod)),VOID);
					//TODO implement while loop naming
				}
				else if(temp=="do"){
					if(f->trim(EOF)) f->error("Uncompleted do-while",true);
					if(f->peek()==':') f->read();
					if(f->trim(EOF)) f->error("Uncompleted do-while",true);
					Statement* blocks = getNextBlock(endWith, mod);
					f->trim(EOF);
					if(f->getNextName(EOF)!="while") f->error("Must complete 'while' part of do{...}while",true);
					bool paren = f->peek()=='(';
					if(paren) f->read();
					Statement* cond = getNextStatement((paren)?EOF:endWith, mod, true,false);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of do-while",true);
					if(!f->done && f->peek()==';') f->read();
					return new DoWhileLoop(pos(), cond, blocks);
				}
				else if(temp=="lambda"){
					if(f->trim(EOF)) f->error("Uncompleted lambda function");
					OModule* module = new OModule(mod);
					std::vector<Declaration*> arguments;
					if(!f->done) arguments = parseArguments(endWith, module, ':');
					if(f->trim(EOF)) f->error("Lambda Function without body");
					if(!f->done && f->peek()==':'){
						f->read();
						if(f->trim(EOF)) f->error("Lambda Function without body (c)");
					}
					Statement* methodBody = getNextBlock(endWith, module);
					return new lambdaFunction(pos(), arguments, methodBody);
				}
				else if (temp == "def" || temp=="function" || temp=="method" ){
					if(f->trim(EOF)) f->error("Uncompleted function");
					Statement* returnName = getNextType(endWith, mod);
					OModule* module = new OModule(mod);
					f->trim(EOF);
					String methodName = "";
					if(isStartName(f->peek())){
						methodName = getNextName(endWith);
						f->trim(EOF);
					}
					std::vector<Declaration*> arguments;
					if(!f->done){
						if(f->peek()=='('){
							f->read();
							arguments = parseArguments(EOF, module);
						}
						else{
							f->trim(endWith);
							methodName="";
							arguments = parseArguments(EOF, module, ':');
						}
					}
					if(f->trim(endWith)) f->error("Function without body");
					if(!f->done && f->peek()==':'){
						f->read();
						if(f->trim(endWith)) f->error("Function without body (c)");
					}
					bool paren;
					Statement* methodBody = getNextBlock(endWith, module, &paren);
					E_VAR* funcName = (methodName=="")?NULL:(new E_VAR(pos(), mod->addPointer(pos(), methodName,NULL,functionClass,NULL,NULL,NULL)));
					userFunction* func = new userFunction(pos(), funcName, returnName, arguments, methodBody);
					f->trim(endWith);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					f->trim(endWith);
					if(opCheck && !semi) return operatorCheck(endWith, mod, func);
					return func;
				}
				else if (temp == "extern"){
					if(f->trim(endWith)) f->error("Extern without name");
					Statement* retV = getNextType(EOF, mod);
					f->trim(endWith);
					E_VAR* externName = getNextVariable(endWith, mod,false);
					externName->pointer->resolveReturnClass() = functionClass;
					f->trim(endWith);
					if(f->peek()!='('){
						f->error("'(' required after extern not "+String(1,f->peek()),true);
					}
					f->read();
					std::vector<Declaration*> dec;
					OModule* m2 = new OModule(mod);
					while(true){
						if(f->trim(endWith) || f->peek()==')') break;
						Declaration* d= getNextDeclaration(EOF, m2);
						dec.push_back(d);
						if(f->trim(endWith)) break;
						if(f->peek()==',') f->read();
					}
					if(f->read()!=')') f->error("Need ending ')' for extern", true);
					return new externFunction(pos(), externName, retV, dec);
					//TODO allow multiple externs
				}
				else if (temp=="true" || temp=="false"){
					Statement* te = new obool(pos(), temp=="true");
					f->trim(endWith);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					f->trim(endWith);
					if(opCheck && !semi)
						return operatorCheck(endWith, mod, te);
					else return te;
				}
				else if(temp=="return"){
					if(!allowDeclaration) f->error("Cannot have return here");
					f->trim(endWith);
					Statement* t = VOID;
					if(f->peek()!='}' && f->peek()!=',' && f->peek()!=':' && f->peek()!=')') t = getNextStatement(endWith, mod, true, false);
					return new E_RETURN(pos(), t, "", RETURN);
				}
				else if(temp=="break" || temp=="continue"){
					if(!allowDeclaration) f->error("Cannot have return here");
					f->trim(endWith);
					String name = "";
					if(isStartName(f->peek())){
						name = getNextName(EOF);
					}
					return new E_RETURN(pos(), NULL, name, (temp=="break")?BREAK:CONTINUE );
				}
				else{
					auto start = f->getMarker();
					f->trim(endWith);
					if(allowDeclaration && start!=f->getMarker() && isStartName(f->peek()) ){
						f->undoMarker(undoRead);
						return getNextDeclaration(endWith, mod);
					} else {
						f->undoMarker(undoRead);
						E_VAR* nextVar = getNextVariable(endWith, mod);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi)
							return operatorCheck(endWith, mod, nextVar);
						else return nextVar;
					}
				}
			}
			else{
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
					case '9':{
						oobject* num = f->readNumber(endWith);
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi) return operatorCheck(endWith, mod, num);
						else return num;
					}
					case '\'':
					case '"':{
						ostring* str = new ostring(pos(), f->readString(endWith));
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi) return operatorCheck(endWith, mod, str);
						else return str;
					}
					case '{':
					case '[':
					case '(':{
						char open = f->read();
						char close = (open=='{')?'}':((open=='[')?']':')');
						f->trim(endWith);
						E_ARR* arr = new E_ARR(pos());
						Statement* temp;
						char te;
						if(f->peek()==close) f->read();
						else{
							temp = getNextStatement(EOF, mod, true,false);
							forceAr = false;
							while(temp->getToken()!=T_EOF){
								arr->values.push_back(temp);
								bool comma = true;
								if(!f->done && f->peek()==','){
									f->read();
									forceAr = true;
								}
								else comma = false;
								if(f->trim(endWith)) f->error("Uncompleted inline array",true);

								if(f->peek()==close) break;
								if(!comma){
									f->error("Missing , in inline array or wrong end char",true);
								}
								if(f->trim(endWith)) f->error("Uncompleted '(' array",true);
								temp = getNextStatement(EOF, mod, true,false);
							}
							if(open=='(' && !forceAr && arr->values.size()==1){
								Statement* temp = arr->values[0];
								//delete arr;
								if((te = f->read())!=close) f->error("Cannot end inline paren with "+
										String(1,te)+" instead of "+String(1,close)
								);
								temp = new E_PARENS(pos(), temp);
								f->trim(endWith);
								semi  = false;
								if(!f->done && f->peek()==';'){ semi = true; }
								f->trim(endWith);
								if(opCheck && !semi) return operatorCheck(endWith, mod, temp);
								else return temp;
							}
							if(f->done) f->error("Uncompleted inline 2");
							if((te = f->read())!=close) f->error("Cannot end inline array with "+
									String(1,te)+" instead of "+String(1,close));
						}
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi) return operatorCheck(endWith, mod, arr);
						else return arr;
					}
					case '+':
					case '-':
					case '~':
					case '!':
					case '*':
					case '&':
						//		case '@':
					{
						char n = f->read();
						if((n=='-' || n=='+') && f->peek()>='0' && f->peek()<='9'){
							if(n=='-') f->write(n);
							Statement* toReturn = f->readNumber(endWith);
							f->trim(endWith);
							semi  = false;
							if(!f->done && f->peek()==';'){ semi = true; }
							f->trim(endWith);
							if(opCheck && !semi)
								toReturn = operatorCheck(endWith, mod, toReturn);
							return toReturn;
						}
						else{
							Statement* toReturn = new E_PREOP(pos(), String(1,n),getNextStatement(endWith, mod, true,false));
							f->trim(endWith);
							semi  = false;
							if(!f->done && f->peek()==';'){ semi = true; }
							f->trim(endWith);
							if(opCheck && !semi)
								toReturn = operatorCheck(endWith, mod, toReturn);
							return toReturn;
						}
					}
				}
			}
			f->error("Unknown rest of file");
			fprintf(stderr, "$$ %d %c (before had %d %c) \n", nex, nex, f->last(), f->last());
			fflush(stderr);
			return VOID;
		}
		Statement* operatorCheck(char endWith, OModule* mod, Statement* exp){
			if(f->done || f->trim(endWith))	return exp;
			if(f->last()==endWith){
				f->write(endWith);
				return exp;
			}
			char tchar = f->peek();
			if(tchar=='{') return exp;
			if(tchar=='['){
				f->read();
				std::vector<Statement*> stack;
				while(true){
					bool comma = true;
					while(!f->done){
						if(f->trim(endWith)) f->error("Indexed operator check hit EOF",true);
						char t = f->peek();
						if(t==',' || t==':'){
							f->read();
							if(t==','){
								exp = getIndex(f, exp,stack);
							}
							else{
								stack.push_back(NULL);
							}

						}
						else if(t==']'){
							comma = false;
							break;
						}
						else break;
					}
					if(f->trim(endWith)) f->error("Uncompleted '[' index",true);
					if(!comma) break;
					stack.push_back(getNextStatement(endWith, mod, true,false));
				}
				if(f->done)	f->error("Uncompleted '[' array 2",true);
				char te;
				if((te = f->read())!=']') f->error("Cannot end '[' array with "+te,true);

				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(endWith);
				if(!semi) return operatorCheck(endWith, mod, getIndex(f, exp, stack));
				else return getIndex(f,exp,stack);
			}
			else if(tchar=='('){
				//TODO parse function args,  cannot do getNextStatement
				// due to operatorCheck on tuple
				Statement* e = getNextStatement(endWith, mod, false,false);
				Statement* ret;
				if(e->getToken()==T_PARENS)
					ret = new E_FUNC_CALL(pos(), exp, std::vector<Statement*>(1,
							((E_PARENS*)e)->inner));
				else ret = new E_FUNC_CALL(pos(), exp, ((E_ARR*)e)->values);
				f->trim(endWith);

				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(endWith);
				if(!semi) return operatorCheck(endWith, mod, ret);
				else return ret;
			}
			else if(tchar=='{'){
				f->read();
				f->trim(endWith);
				cerr << " '{' operatorCheck not implemented yet" << endl << flush;
				exit(0);
			}
			else if (tchar=='?'){
				f->read();
				f->trim(endWith);
				Statement* op1 = getNextStatement(EOF, mod, true, false);
				f->trim(endWith);
				if(f->read()!=':') f->error("Ternary operator requires ':'",true);
				Statement* op2 = getNextStatement(endWith, mod, true, false);
				return new TernaryOperator(pos(), exp, op1, op2);
			}
			//TODO implement generics
			/*
			else if(tchar=='<'){
				f->read();
				tchar = f->peek();
				if(tchar=='<' || tchar=='=' || tchar=='>'){
					f->write(tchar);
					f->write('<');
				}
				else{
					f->trim(endWith);
					String name = f->getNextName(endWith);
					if(name.length()==0){
						f->writeString(name);
						f->write(' ');

					}
					tchar = f->peek();
				}
			}*/
			String tmp = f->getNextOperator(endWith);

			if(tmp.length()==0) return exp;
			Statement* fixed;

			if(tmp=="!") {
				return new E_POSTOP(pos(), "!", exp);
			}
			//TODO implement generics
			/*
			else if (tmp == "<"){
				//equality and check
			}
			 */ //TODO implement custom operators (a and b,  r if g else b )
			else if (in<String>(TYPE_OPERATORS, tmp)){
				String name = getNextName(endWith);
				if(name.length()==0) f->error("Name for lookup cannot be "+name);
				fixed = (new E_LOOKUP(pos(), exp, name, tmp));
			}
			else if(tmp=="="){
				Statement* post = getNextStatement(endWith, mod, true,false);
				fixed = new E_SET(pos(), exp, post);
			}
			else if(tmp=="++" || tmp=="--"){
				fixed = new E_POSTOP(pos(), tmp, exp);
			}
			else if(tmp.size()>=2 && tmp[tmp.size()-1]=='=' && !(tmp[tmp.size()-2]=='=' || tmp[tmp.size()-2]=='!' || (tmp.size()==2 && (tmp[0]=='<' || tmp[0]=='>')))){
				tmp = tmp.substr(0,tmp.size()-1);
				Statement* post = getNextStatement(endWith, mod, true,false);
				fixed = new E_SET(pos(), exp, new E_BINOP(pos(), exp, post, tmp));
			}
			else{
				Statement* post = getNextStatement(endWith, mod, true,false);
				if(post->getToken()==T_VOID)
					fixed = new E_POSTOP(pos(), tmp, exp);
				else fixed = (new E_BINOP(pos(), exp, post,tmp))->fixOrderOfOperations();
			}
			f->trim(endWith);
			bool semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			f->trim(endWith);
			if(!semi) fixed = operatorCheck(endWith, mod, fixed);
			return fixed;
		}
};

#endif /* LEXER_HPP_ */
