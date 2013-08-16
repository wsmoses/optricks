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
#include "expressions/E_BINOP.hpp"
#include "expressions/E_UOP.hpp"
#include "expressions/E_VAR.hpp"
#include "primitives/oobject.hpp"

class Lexer{
	public:
		Stream* f;
		char endWith;
		Module * module;

		RData rdata;
		Lexer(Stream* t, char e):f(t),endWith(e),rdata(){
			module = new Module("__main__",getGlobalContext());
		}
		std::vector<Declaration*> parseArguments(OModule* m, char finish=')'){
			std::vector<Declaration*> args;
			if(f->done) return args;
			while(true){

				if(f->trim(endWith)) return args;
				if(f->peek()==finish){
					f->read();
					return args;
				}
				Declaration* d = dynamic_cast<Declaration*>(getNextStatement(m, true,true));
				if(d==NULL){
					f->error("Could not parse declaration",true);
				}
				for(auto& a:args){
					if(d->variable->pointer->name == a->variable->pointer->name){
						f->error("Cannot have duplicate argument name: "+a->variable->pointer->name, true);
					}
				}
				args.push_back(d);
				f->trim(endWith);
				/*
				String type = f->getNextName(endWith);

				if(f->trim(endWith)){
					if(type.size()>0)
						args.push_back(new Declaration(new E_VAR(type)));
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
					defVal = getNextExpression(true);
					if(defVal->getToken()==T_EOF) f->error("Could not parse default arg");
				}


				args.push_back(new DefaultDeclaration(new E_VAR(name), (type.length()>0)?(new E_VAR(type)):NULL, defVal));
				*/
				char tchar = f->peek();
				if(tchar==finish || tchar==',' || tchar==';'){
					f->read();
					if(tchar==finish) return args;
				}
				else f->error("Could not parse arguments");
			}
			return args;
		}
		Statement* getNextBlock(OModule* m, bool*par=NULL){
			OModule* module = new OModule(m);
			f->trim(endWith);
			bool paren = f->peek()=='{';
			if(paren) f->read();
			if(par!=NULL) *par=paren;
			if(paren){
				Block* blocks = new Block();
				bool fpek;
				do{
					f->trim(endWith);
					Statement* e = getNextStatement(module, true, true);
					if(e->getToken()!=T_EOF) blocks->values.push_back(e);
					f->trim(endWith);
					while(!f->done && f->peek()==';'){f->read();f->trim(endWith);}
					fpek = f->peek()=='}';
				}while(!f->done && !fpek);
				if(fpek) f->trim(endWith);
				f->trim(endWith);
				if(paren && f->read()!='}') f->error("Need '}' for ending block statement");
				return blocks;
			}
			else{
				Statement* s = getNextStatement(module, true, true);
				while(!f->done && f->peek()==';'){f->read();f->trim(endWith);}
				f->trim(endWith);
				return s;
			}
		}
		Expression* getNextExpression(OModule* m, bool opCheck){
			Statement* s = getNextStatement(m, opCheck, false);
			return dynamic_cast<Expression*>(s);
		}
		Statement* getNextStatement(OModule* mod, bool opCheck, bool allowDeclaration){
			if(f->done || f->trim(endWith)) return VOID;
			int nex = f->peek();
			if(f->done || nex==EOF || nex==endWith) return VOID;
			if(nex==';') return VOID;
			bool semi;
			if (isalpha(nex) || nex=='_' || nex=='$') {
				String temp = f->getNextName(endWith);
				if(temp=="if"){
					if(f->trim(endWith)) f->error("Uncompleted if");
					std::vector<std::pair<Expression*,Statement* >> statements;
					Expression* c = getNextExpression(mod, true);
					if(!f->done && f->peek()==':') f->read();
					Statement* s = getNextBlock(mod);
					statements.push_back(
							std::pair<Expression*,Statement* >(c,s));
					f->trim(endWith);
					while(!f->done && f->peek()==';') f->read();
					String test = f->getNextName(endWith);
					Statement* finalElse = VOID;
					while(!f->done && (test=="else" || test=="elif")){
						f->trim(endWith);
						bool elif = test=="elif";
						if(!elif){
							auto m = f->getMarker();
							String yy = f->getNextName(endWith);
							if(yy=="if"){ elif=true; f->trim(endWith); }
							else { f->undoMarker(m); }
						}
						if(elif){
							c = getNextExpression(mod, true);
							if(!f->done && f->peek()==':') f->read();
							s = getNextBlock(mod);
							statements.push_back(
									std::pair<Expression*,Statement* >(c,s));
							f->trim(endWith);

							while(!f->done && f->peek()==';') f->read();
						}
						else{
							if(!f->done && f->peek()==':') f->read();
							f->trim(endWith);
							finalElse = getNextBlock(mod);
							f->trim(endWith);
							while(!f->done && f->peek()==';') f->read();
						}
						test = f->getNextName(endWith);
					}
					f->write(test);
					return new IfStatement(statements,finalElse);
				}
				else if(temp=="for"){
					if(f->trim(endWith)) f->error("Uncompleted for",true);
					bool paren = f->peek()=='(';
					//Standard for(i = 0; i<7; i++)
					OModule* module = new OModule(mod);
					if(paren){
						f->read();
						f->trim(endWith);
						Statement* init = getNextStatement(module, true, true);
						if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
						Statement* scond = getNextStatement(module, true, false);
						Expression* cond;
						if(scond->getToken()==T_VOID) cond = new obool(true);
						else cond = dynamic_cast<Expression*>(scond);
						if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
						Statement* inc = getNextStatement(module, true, false);
						f->trim(endWith);
						if(f->read()!=')') f->error("Invalid additional piece of for loop",true);
						Statement* blocks = getNextBlock(module);
						return new ForLoop(init,cond,inc,blocks);
						//TODO implement for loop naming
					}
					else{
						String varName = f->getNextName(endWith);
						if( varName.length()==0)f->error("Need variable to iterate on",true);
						f->trim(endWith);
						bool as = f->getNextName(endWith)=="in";
						f->trim(endWith);
						bool col = false;
						if(!f->done && f->peek()==':'){
							f->read(); f->trim(endWith);
						}
						if(as==col){
							f->error("Need either ':' or 'in' to separate iterator variable from iterable");
						}
						Expression* iterable = getNextExpression(mod, true);
						f->trim(endWith);
						if(paren){
							if(f->read()!=')') f->error("Need ')' for for loop ");
						}
						f->trim(endWith);
						if(!f->done && f->peek()==':'){
							f->read();
							f->trim(endWith);
						}
						Statement* blocks = getNextBlock(module);
						return new ForEachLoop(new E_VAR(module->addPointer(varName,NULL,NULL,NULL)),iterable,blocks,"");
						//TODO implement for loop naming
					}
				}
				else if(temp=="while"){
					if(f->trim(endWith)) f->error("Uncompleted while",true);
					bool paren = f->peek()=='(';
					if(paren) f->read();
					Expression* cond = getNextExpression(mod, true);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of while",true);
					if(f->trim(endWith)) f->error("Uncompleted do-while",true);
					if(f->peek()==':') f->read();
					return new WhileLoop(cond,getNextBlock(mod));
					//TODO implement while loop naming
				}
				else if(temp=="do"){
					if(f->trim(endWith)) f->error("Uncompleted do-while",true);
					if(f->peek()==':') f->read();
					if(f->trim(endWith)) f->error("Uncompleted do-while",true);
					Statement* blocks = getNextBlock(mod);
					f->trim(endWith);
					if(f->getNextName(endWith)!="while") f->error("Must complete 'while' part of do{...}while",true);
					bool paren = f->peek()=='(';
					if(paren) f->read();
					Expression* cond = getNextExpression(mod, true);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of do-while",true);
					cout << "Do-While("<<cond<<", "<< blocks << endl << flush;
					f->error("do-while is not fully implemented yet",true);
					//TODO implement do loop naming
				}
				else if (temp == "def" || temp=="lambda" || temp=="function" || temp=="method" ){
					if(f->trim(endWith)) f->error("Uncompleted function");
					String returnName = f->getNextName(endWith);
					OModule* module = new OModule(mod);
					f->trim(endWith);
					unsigned int m = f->getMarker();
					String methodName = f->getNextName(endWith);
					if(f->trim(endWith)) f->error("Uncompleted function (with name)");
					std::vector<Declaration*> arguments;
					if(!f->done){
						if(f->peek()=='('){
							f->read();
							arguments = parseArguments(mod);
						}
						else{
							f->undoMarker(m);
							f->trim(endWith);
							methodName="";
							arguments = parseArguments(mod, ':');
						}
					}
					if(f->trim(endWith)) f->error("Function without body");
					if(!f->done && f->peek()==':'){
						f->read();
						if(f->trim(endWith)) f->error("Function without body (c)");
					}
					bool paren;
					Statement* methodBody = getNextBlock(mod, &paren);
					if(paren){
						//TODO regular function
						cout << "FUNC(" << methodName << "," << arguments << ", " << methodBody << endl << flush;
						f->error("Function not implemented",true);//TODO re-implement
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						//if(opCheck && !semi)
						//	toReturn = operatorCheck(f, toReturn, endWith);
						return VOID;//TODO fix this and two lines above when functino is implemented
					}
					else{
						//TODO lambda function
						cout << "LAMBDA(" << methodName << "," << arguments << ", " << methodBody << ")" << endl << flush;
						exit(0);
						//toReturn = new lambdaFunction(methodName, arguments, (*methodBody)[0]);
						delete methodBody;
					}
				}
				else if (temp == "extern"){
					if(f->trim(endWith)) f->error("Extern without name");
					LateResolve* retV =
							new LateResolve(mod,f->getNextName(endWith))
									;
					f->trim(endWith);
					Resolvable* externName = mod->addPointer(f->getNextName(endWith),NULL,NULL,NULL);
					if(f->peek()!='(') f->error("'(' required after extern",true);
					f->read();
					std::vector<Declaration*> dec;
					while(true){
						if(f->trim(endWith) || f->peek()==')') break;
						Statement* s = getNextStatement(NULL,true,true);//TODO check
						Declaration* d = dynamic_cast<Declaration*>(s);
						if(d==NULL) f->error("Could not parse extern declaration");
						dec.push_back(d);
					}
					return new externFunction(externName, retV, dec);
					/*
					std::vector<String> vals = f->getCommaSeparated(endWith);

					if(!f->done && f->peek()==';') f->read();
					if(vals.size()>1){
						Block* arr = new Block();
						for (auto &a: vals){
							arr->values.push_back(new E_EXT(a));
						}
						return arr;
					}
					else if (vals.size()>0){
						return new E_EXT(vals[0]);
					}*/
					//toReturn->write(cout) << endl;
					//if(opCheck)
					//toReturn = operatorCheck(f, toReturn,endWith);
				}
				else if (temp=="true" || temp=="false"){
					Expression* te = new obool(temp=="true");
					f->trim(endWith);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					f->trim(endWith);
					if(opCheck && !semi)
						return operatorCheck(mod, te);
					else return te;
				}
				else{
					Expression* te = new E_VAR(new LateResolve(mod,temp));
					auto start = f->getMarker();
					f->trim(endWith);
					if(allowDeclaration && start!=f->getMarker()){
						E_VAR* n = dynamic_cast<E_VAR*>(getNextStatement(mod, false, false));
						if(n!=NULL){
							f->trim(endWith);
							Expression* value = NULL;
							if(f->peek()=='='){
								f->read();
								value = getNextExpression(mod, true);
							}
							if(!f->done && f->peek()==';'){ semi = true; }
							return new Declaration((E_VAR*)te, n, value);
						}
						else{
							f->undoMarker(start);
							f->trim(endWith);
						}
					}
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					f->trim(endWith);
					if(opCheck && !semi)
						return operatorCheck(mod, te);
					else return te;
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
						if(opCheck && !semi) return operatorCheck(mod, num);
						else return num;
					}
					case '\'':
					case '"':{
						ostring* str = new ostring(f->readString(endWith));
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi) return operatorCheck(mod, str);
						else return str;
					}
					case '{':
					case '[':
					case '(':{
						char open = f->read();
						char close = (open=='{')?'}':((open=='[')?']':')');
						f->trim(endWith);
						E_ARR* arr = new E_ARR();
						Expression* temp;
						char te;
						if(f->peek()==close) f->read();
						else{
							temp = getNextExpression(mod, true);
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
								temp = getNextExpression(mod, true);
							}
							if(open=='(' && !forceAr && arr->values.size()==1){
								Expression* temp = arr->values[0];
								delete arr;
								if((te = f->read())!=close) f->error("Cannot end inline paren with "+
										String(1,te)+" instead of "+String(1,close)
								);
								temp = new E_PARENS(temp);
								f->trim(endWith);
								semi  = false;
								if(!f->done && f->peek()==';'){ semi = true; }
								f->trim(endWith);
								if(opCheck && !semi) return operatorCheck(mod, temp);
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
						if(opCheck && !semi) return operatorCheck(mod, arr);
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
							Expression* toReturn = f->readNumber(endWith);
							f->trim(endWith);
							semi  = false;
							if(!f->done && f->peek()==';'){ semi = true; }
							f->trim(endWith);
							cout << "opcheck " << (opCheck && !semi) << endl << flush;
							if(opCheck && !semi)
								toReturn = operatorCheck(mod, toReturn);
							return toReturn;
						}
						else{
						Expression* toReturn = new E_PREOP(String(1,n),getNextExpression(mod, true));
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi)
							toReturn = operatorCheck(mod, toReturn);
						return toReturn;
						}
					}
				}
			}
			f->error("Unknown rest of file");
			fprintf(stderr, "$$ %d %c\n", nex, nex);
			fflush(stderr);
			return VOID;
		}
		Expression* operatorCheck(OModule* mod, Expression* exp){
			if(f->done || f->trim(endWith)){
				return exp;
			}
			char tchar = f->peek();
			if(tchar=='['){
				f->read();
				std::vector<Expression*> stack;
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
					stack.push_back(getNextExpression(mod, true));
				}
				if(f->done)	f->error("Uncompleted '[' array 2",true);
				char te;
				if((te = f->read())!=']') f->error("Cannot end '[' array with "+te,true);

				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(endWith);
				if(!semi) return operatorCheck(mod, getIndex(f, exp, stack));
				else return getIndex(f,exp,stack);
			}
			else if(tchar=='('){
				//TODO parse function args,  cannot do getNextExpression
				// due to operatorCheck on tuple
				Expression* e = getNextExpression(mod, false);
				Expression* ret;
				E_VAR* fun = dynamic_cast<E_VAR*>(exp);
				//TODO support E_Lookup
				if(fun==NULL) f->error("Cannot get ofunction to call",true);
				if(e->getToken()==T_PARENS){
					ret = new E_FUNC_CALL(fun->pointer, std::vector<Expression*>(1,
							((E_PARENS*)e)->inner));
				}
				else{
					ret = new E_FUNC_CALL(fun->pointer, ((E_ARR*)e)->values);
				}
				f->trim(endWith);

				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(endWith);
				if(!semi) return operatorCheck(mod, ret);
				else return ret;
			}
			else if(tchar=='{'){
				f->read();
				f->trim(endWith);
				cerr << " '{' operatorCheck not implemented yet" << endl << flush;
				exit(0);
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
			Expression* fixed;

			if(tmp=="!") {
				//TODO implement factorial
				f->error("Factorial not implemented yet",true);
			}
			//TODO implement generics
			/*
			else if (tmp == "<"){
				//equality and check
			}
			 */ //TODO implement custom operators (a and b,  r if g else b )
			else if (tmp == "?"){
				f->error("Terenary operator not implement",true);
				//TODO implement terenary
			}
			else if (tmp == "." || tmp=="->" || tmp==":" || tmp=="::" || tmp==".*"
					|| tmp==":*" || tmp=="::*"|| tmp=="->*" || tmp=="=>*"){
				String name = f->getNextName(endWith);
				if(name.length()==0) f->error("Name for lookup cannot be "+name);

				fixed = (new E_LOOKUP(tmp, exp, name));
			}
			else if(tmp=="="){
				Expression* post = getNextExpression(mod, true);
				//	if(tmp->getToken()!=T_VAR)
				fixed = new E_PARENS(new E_BINOP(exp, post,tmp));
			}
			else{
				Expression* post = getNextExpression(mod, true);
				fixed = (new E_BINOP(exp, post,tmp))->fixOrderOfOperations();
			}
			f->trim(endWith);
			bool semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			f->trim(endWith);
			if(!semi) fixed = operatorCheck(mod, fixed);
			return fixed;
		}
};
#endif /* LEXER_HPP_ */
