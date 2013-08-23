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
		char endWith;

		RData rdata;
		Lexer(Stream* t, char e):f(t),endWith(e),rdata(){
			myMod = new OModule(LANG_M);
		}
		void execFile(String fileName, bool newModa, bool newModb, FILE* fi=NULL){
			if(fi==NULL) fi = fopen(fileName.c_str(), "r");
			char tt = endWith;
			endWith = EOF;
			Stream* tmp = f;
			Stream next(fi,false);
			f = &next;
			if(newModa) myMod = new OModule(myMod);
			std::vector<Statement*> stats;
			while(true){
				while(f->peek()==';') f->read();
				Statement* s = getNextStatement();
				if(s==NULL || s->getToken()==T_VOID) break;
				stats.push_back(s->simplify());
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
			for(auto& n: stats) n->evaluate(rdata);
			rdata.builder.CreateRetVoid();
			verifyFunction(*F);
			rdata.fpm->run(*F);
			void *FPtr = rdata.exec->getPointerToFunction(F);
			void (*FP)() = (void (*)())(intptr_t)FPtr;
			FP();
			fclose(fi);
			f = tmp;
			endWith = tt;
			if(newModb) myMod = new OModule(myMod);
		}
		Statement* getNextStatement(){
			return getNextStatement(myMod, true, true);
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

				Statement* defVal = NULL;

				if(!f->done && f->peek()=='='){
					f->read();
					defVal = getNextStatement(true);
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
		PositionID pos(){
			return f->pos();
		}
		Statement* getNextBlock(OModule* m, bool*par=NULL){
			OModule* module = new OModule(m);
			f->trim(endWith);
			bool paren = f->peek()=='{';
			if(paren) f->read();
			if(par!=NULL) *par=paren;
			if(paren){
				Block* blocks = new Block(pos());
				bool fpek;
				do{
					f->trim(endWith);
					Statement* e = getNextStatement(module, true, true);
					if(e!=NULL && e->getToken()!=T_VOID && e->getToken()!=T_EOF) blocks->values.push_back(e);
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
					std::vector<std::pair<Statement*,Statement* >> statements;
					Statement* c = getNextStatement(mod, true,false);
					if(!f->done && f->peek()==':') f->read();
					Statement* s = getNextBlock(mod);
					statements.push_back(
							std::pair<Statement*,Statement* >(c,s));
					f->trim(endWith);
					while(!f->done && f->peek()==';') f->read();
					auto marker = f->getMarker();
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
							c = getNextStatement(mod, true,false);
							if(!f->done && f->peek()==':') f->read();
							s = getNextBlock(mod);
							statements.push_back(
									std::pair<Statement*,Statement* >(c,s));
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
						Statement* cond;
						if(scond->getToken()==T_VOID) cond = new obool(pos(), true);
						else cond = dynamic_cast<Statement*>(scond);
						if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
						Statement* inc = getNextStatement(module, true, false);
						f->trim(endWith);
						if(f->read()!=')') f->error("Invalid additional piece of for loop",true);
						Statement* blocks = getNextBlock(module);
						return new ForLoop(pos(), init,cond,inc,blocks);
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
						Statement* iterable = getNextStatement(mod, true,false);
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
						f->error("Implement for-each loop");
						//return new ForEachLoop(new E_VAR(module->addPointer(varName,NULL,NULL,NULL)),iterable,blocks,"");
						//TODO implement for loop naming
					}
				}
				else if(temp=="while"){
					if(f->trim(endWith)) f->error("Uncompleted while",true);
					bool paren = f->peek()=='(';
					if(paren) f->read();
					Statement* cond = getNextStatement(mod, true,false);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of while",true);
					if(f->trim(endWith)) f->error("Uncompleted do-while",true);
					if(f->peek()==':') f->read();
					return new WhileLoop(pos(), cond,getNextBlock(mod));
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
					Statement* cond = getNextStatement(mod, true,false);
					if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of do-while",true);
					cout << "Do-While("<<cond<<", "<< blocks << endl << flush;
					f->error("do-while is not fully implemented yet",true);
					//TODO implement do loop naming
				}
				else if(temp=="lambda"){
					if(f->trim(endWith)) f->error("Uncompleted lambda function");
					OModule* module = new OModule(mod);
					std::vector<Declaration*> arguments;
					if(!f->done) arguments = parseArguments(module, ':');
					if(f->trim(endWith)) f->error("Lambda Function without body");
					if(!f->done && f->peek()==':'){
						f->read();
						if(f->trim(endWith)) f->error("Lambda Function without body (c)");
					}
					Statement* methodBody = getNextBlock(module);
					return new lambdaFunction(pos(), arguments, methodBody);
				}
				else if (temp == "def" || temp=="lambda" || temp=="function" || temp=="method" ){
					if(f->trim(endWith)) f->error("Uncompleted function");
					Statement* returnName = getNextStatement(mod, true, false);
					OModule* module = new OModule(mod);
					f->trim(endWith);
					unsigned int m = f->getMarker();
					String methodName = f->getNextName(endWith);
					if(f->trim(endWith)) f->error("Uncompleted function (with name)");
					std::vector<Declaration*> arguments;
					if(!f->done){
						if(f->peek()=='('){
							f->read();
							arguments = parseArguments(module);
						}
						else{
							f->undoMarker(m);
							f->trim(endWith);
							methodName="";
							arguments = parseArguments(module, ':');
						}
					}
					if(f->trim(endWith)) f->error("Function without body");
					if(!f->done && f->peek()==':'){
						f->read();
						if(f->trim(endWith)) f->error("Function without body (c)");
					}
					bool paren;
					Statement* methodBody = getNextBlock(module, &paren);
					E_VAR* funcName = new E_VAR(pos(), mod->addPointer(pos(), methodName,NULL,functionClass,NULL,NULL,NULL));
					userFunction* func = new userFunction(pos(), funcName, returnName, arguments, methodBody);
						f->trim(endWith);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						f->trim(endWith);
						if(opCheck && !semi) return operatorCheck(mod, func);
						return func;
				}
				else if (temp == "extern"){
					if(f->trim(endWith)) f->error("Extern without name");
					Statement* retV = getNextStatement(mod, true, false);
					f->trim(endWith);
					E_VAR* externName = new E_VAR(pos(), mod->addPointer(pos(), f->getNextName(endWith),NULL,functionClass,NULL,NULL,NULL));
					f->trim(endWith);
					if(f->peek()!='('){
						f->error("'(' required after extern not "+String(1,f->peek()),true);
					}
					f->read();
					std::vector<Declaration*> dec;
					OModule* m2 = new OModule(mod);
					while(true){
						if(f->trim(endWith) || f->peek()==')') break;
						Statement* s = getNextStatement(m2,true,true);
						Declaration* d = dynamic_cast<Declaration*>(s);
						if(d==NULL) f->error("Could not parse extern declaration");
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
						return operatorCheck(mod, te);
					else return te;
				}
				else if(temp=="return"){
					f->trim(endWith);
					return new E_RETURN(pos(), getNextStatement(mod, true, false));
				}
				else{
					Statement* te = new E_VAR(pos(), new LateResolve(mod,temp,pos()));
					auto start = f->getMarker();
					f->trim(endWith);
					if(allowDeclaration && start!=f->getMarker() && isalpha(f->peek()) ){
						E_VAR* n = dynamic_cast<E_VAR*>(getNextStatement(mod, false, false));
						if(n!=NULL){
							f->trim(endWith);
							Statement* value = NULL;
							if(f->peek()=='='){
								f->read();
								value = getNextStatement(mod, true,false);
							}
							if(!f->done && f->peek()==';'){ semi = true; }
							n = new E_VAR(pos(), mod->addPointer(pos(), n->pointer->name,NULL,NULL,NULL,NULL,NULL)); // TODO look at
							return new Declaration(pos(), (E_VAR*)te, n, value);
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
						ostring* str = new ostring(pos(), f->readString(endWith));
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
						E_ARR* arr = new E_ARR(pos());
						Statement* temp;
						char te;
						if(f->peek()==close) f->read();
						else{
							temp = getNextStatement(mod, true,false);
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
								temp = getNextStatement(mod, true,false);
							}
							if(open=='(' && !forceAr && arr->values.size()==1){
								Statement* temp = arr->values[0];
								delete arr;
								if((te = f->read())!=close) f->error("Cannot end inline paren with "+
										String(1,te)+" instead of "+String(1,close)
								);
								temp = new E_PARENS(pos(), temp);
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
							Statement* toReturn = f->readNumber(endWith);
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
							Statement* toReturn = new E_PREOP(pos(), String(1,n),getNextStatement(mod, true,false));
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
		Statement* operatorCheck(OModule* mod, Statement* exp){
			if(f->done || f->trim(endWith)){
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
					stack.push_back(getNextStatement(mod, true,false));
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
				//TODO parse function args,  cannot do getNextStatement
				// due to operatorCheck on tuple
				Statement* e = getNextStatement(mod, false,false);
				Statement* ret;
					if(e->getToken()==T_PARENS)
						ret = new E_FUNC_CALL(pos(), exp, std::vector<Statement*>(1,
								((E_PARENS*)e)->inner));
					else ret = new E_FUNC_CALL(pos(), exp, ((E_ARR*)e)->values);
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
			else if (tchar=='?'){
				f->read();
				f->trim(endWith);
				Statement* op1 = getNextStatement(mod, true, false);
				f->trim(endWith);
				if(f->read()!=':') f->error("Ternary operator requires ':'",true);
				Statement* op2 = getNextStatement(mod, true, false);
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
			else if (tmp == "." || tmp=="->" || tmp==":" || tmp=="::" || tmp==".*"
					|| tmp==":*" || tmp=="::*"|| tmp=="->*" || tmp=="=>*"){
				String name = f->getNextName(endWith);
				if(name.length()==0) f->error("Name for lookup cannot be "+name);

				fixed = (new E_LOOKUP(pos(), tmp, exp, name));
			}
			else if(tmp=="="){
				Statement* post = getNextStatement(mod, true,false);
				fixed = new E_SET(pos(), exp, post);
			}
			else{
				Statement* post = getNextStatement(mod, true,false);
				fixed = (new E_BINOP(pos(), exp, post,tmp))->fixOrderOfOperations();
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
