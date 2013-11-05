/*
 * Lexer.hpp
 *
 *  Created on: Aug 6, 2013
 *      Author: wmoses
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include "O_Stream.hpp"
#include "constructs/Declaration.hpp"
#include "constructs/WhileLoop.hpp"
#include "constructs/ForLoop.hpp"
#include "constructs/ForEachLoop.hpp"
#include "constructs/IfStatement.hpp"
#include "constructs/Block.hpp"
#include "constructs/ImportStatement.hpp"
#include "expressions/E_RETURN.hpp"
#include "expressions/E_BINOP.hpp"
#include "expressions/E_UOP.hpp"
#include "expressions/E_VAR.hpp"
#include "expressions/E_SET.hpp"
#include "expressions/E_GEN.hpp"
#include "expressions/E_TUPLE.hpp"
#include "expressions/E_TERNARY.hpp"
#include "primitives/oobject.hpp"
#include "primitives/obool.hpp"
#include "primitives/ochar.hpp"

enum ParseLoc{
	PARSE_GLOBAL = 1,
	PARSE_LOCAL = 2,
	PARSE_EXPR = 3
};
class ParseData{
	public:
		char endWith;
		OModule* mod;
		bool operatorCheck;
		ParseLoc loc;
		ParseData(char a, OModule* b, bool c, ParseLoc d):endWith(a),mod(b),operatorCheck(c),loc(d){};
		inline bool allowsDec() const{
			return loc==PARSE_GLOBAL || loc==PARSE_LOCAL;
		}
		inline ParseData getEndWith(char c) const{
			return ParseData(c, mod, operatorCheck, loc);
		}
		inline ParseData getLoc(ParseLoc c) const{
			return ParseData(endWith,mod, operatorCheck, c);
		}
		inline ParseData getModule(OModule* c) const{
			return ParseData(endWith,c, operatorCheck, loc);
		}
		inline ParseData getOperator(bool c) const{
			return ParseData(endWith,mod,c, loc);
		}
};
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
		std::vector<String> visitedFiles;
		virtual ~Lexer(){};
		void getStatements(bool global, bool debug, std::vector<String> fileNames,std::vector<Statement*>& stats){
			while(fileNames.size()>0){
				String fileName = fileNames.back();
				fileNames.pop_back();
				char cwd[1024];
				if(getcwd(cwd,sizeof(cwd))==NULL) pos().error("Could not determine Current Working Directory");
				String dir, file;
				getDir(fileName, dir, file);
				if(dir!="." && chdir(dir.c_str())!=0) pos().error("Could not change directory to "+dir+"/"+file);
				//cout << "Opened: " << dir << "/" << file << endl << flush;
				Stream* tmp = f;
				Stream next(file,false);
				f = &next;
				while(true){
					while(f->peek()==';') f->read();
					Statement* s = getNextStatement(EOF,global);
					if(s==NULL || s->getToken()==T_VOID) break;
					if(s->getToken()==T_IMPORT){
						ImportStatement* import = (ImportStatement*)s;
						std::vector<String> pl = {import->toImport};
						getStatements(global, debug, pl, stats);
						continue;
					}
					if(debug && s->getToken()!=T_VOID){
						std::cout << s << ";" << endl << endl << flush;
					}
					s = s->simplify();
					stats.push_back(s);
				}
				f->close();
				f = tmp;
				if(chdir(cwd)!=0) pos().error("Could not change directory back to "+String(cwd));
			}
		}
		void execFiles(bool global, std::vector<String> fileNames, raw_ostream* file, bool debug, int toFile=0,unsigned int optLevel = 3){
			std::vector<Statement*> stats;
			getStatements(global, debug, fileNames, stats);
			for(auto& n: stats) n->registerClasses(rdata);
			for(auto& n: stats){
				n->registerFunctionPrototype(rdata);
			}
			for(auto& n: stats){
				n->buildFunction(rdata);
			}
			for(auto& n: stats) n->checkTypes(rdata);

			FunctionType *FT = FunctionType::get(VOIDTYPE, std::vector<Type*>(), false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, "main", rdata.lmod);
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
			rdata.builder.SetInsertPoint(BB);
			for(auto& n: stats) n->evaluate(rdata);
			rdata.builder.CreateRetVoid();
			if(debug){
				this->myMod->write(cerr,"");
				rdata.lmod->dump();
				cerr << endl << flush;
			}
			verifyFunction(*F);
			if(toFile>0) verifyModule(*(rdata.lmod));
			//auto modOpt = new PassManager();
			//FunctionPassManager* fnOpt = new FunctionPassManager(rdata.lmod);

			// Set up optimisers
			//PassManagerBuilder pmb;
			//pmb.OptLevel = optLevel;
			//pmb.populateFunctionPassManager(*fnOpt);
			//if(toFile>0) pmb.populateModulePassManager(*modOpt);
			//fnOpt->run(*F);
			//if(toFile>0) modOpt->run(*(rdata.lmod));
			rdata.fpm->run(*F);
			if(toFile>0) rdata.mpm->run(*(rdata.lmod));

			if(debug){
				//rdata.lmod->dump();
			}
			if(toFile==3){
				//				llvm::raw_os_ostream raw_stream(file);

				WriteBitcodeToFile(rdata.lmod, *file);
				//				rdata.lmod->print(raw_stream, 0);
			} else if(toFile==2){
				//				llvm::raw_os_ostream raw_stream(file);
				rdata.lmod->print(*file,0);
				//				WriteBitcodeToFile(rdata.lmod, file);
				//				rdata.lmod->print(raw_stream, 0);
			} else {
				void *FPtr = rdata.exec->getPointerToFunction(F);
				void (*FP)() = (void (*)())(intptr_t)FPtr;
				FP();
			}
		}
		String getNextName(char endWith){
			f->trim(endWith);
			auto temp = f->getNextName(endWith);
			if(temp.length()==0) f->error("Variable Name of Length 0, next char is "+String(1,f->peek()));
			if(in<String>(RESERVED_KEYWORDS, temp)) f->error("Variable name is a reserved keyword");
			if(in<String>(BINARY_OPERATORS, temp)) f->error("Variable name is a binary operator");
			return temp;
		}
		E_VAR* getNextVariable(ParseData data, bool late=true){
			Resolvable* pointer;
			if(late) pointer = new LateResolve(data.mod, getNextName(data.endWith), pos());
			else pointer = data.mod->addPointer(pos(), getNextName(data.endWith),DATA::getNull());
			return new E_VAR(pos(), pointer);
		}
		Declaration* getNextDeclaration(ParseData data,bool global=false,bool allowAuto=false){
			trim(data);
			Statement* declarationType = getNextType(data.getEndWith(EOF));
			trim(EOF);
			String varName;
			if(allowAuto && (declarationType->getToken()==T_VAR) && !isStartName(f->peek())){
				varName = ((E_VAR*)(declarationType))->pointer->name;
				declarationType = new ClassProtoWrapper(autoClass);
			} else varName = getNextName(data.endWith);
			trim(data);
			Statement* value = NULL;
			if(f->peek()=='='){
				f->read();
				value = getNextStatement(data.getLoc(PARSE_EXPR));
			}
			E_VAR* variable = new E_VAR(pos(), data.mod->addPointer(pos(), varName,DATA::getNull()));
			return new Declaration(pos(), declarationType, variable, global || (data.loc==PARSE_GLOBAL), value);
		}
		Statement* getNextStatement(char endWith,bool global){
			return getNextStatement(ParseData(endWith,myMod,true,global?PARSE_GLOBAL:PARSE_LOCAL));
		}
		std::vector<Declaration*> parseArguments(ParseData data, char finish=')'){
			std::vector<Declaration*> args;
			if(f->done) return args;
			while(true){
				trim(data);
				if(f->peek()==finish){
					f->read();
					return args;
				}
				Declaration* d = getNextDeclaration(data.getEndWith(EOF),false,true);
				for(auto& a:args){
					if(d->variable->pointer->name == a->variable->pointer->name){
						f->error("Cannot have duplicate argument name: "+a->variable->pointer->name, true);
					}
				}
				args.push_back(d);
				trim(data);
				char tchar = f->peek();
				if(tchar==finish || tchar==','){
					f->read();
					if(tchar==finish) return args;
				}
				else f->error("Could not parse arguments - encountered character "+String(1,tchar));
			}
			return args;
		}
		inline PositionID pos() const{
			assert(f!=NULL);
			return f->pos();
		}
		inline void trim(char c){
			//		if(
			assert(f!=NULL);
			f->trim(c)
									//) f->error("Found EOF while parsing")
											;
		}
		inline void trim(ParseData data){
			//	if(
			f->trim(data.endWith)
									//) f->error("Found EOF while parsing")
											;
		}
		Statement* getNextBlock(ParseData data, bool*par=NULL){
			OModule* module = new OModule(data.mod);
			f->trim(EOF);
			bool paren = f->peek()=='{';
			if(paren) f->read();
			if(par!=NULL) *par=paren;
			if(paren){
				Block* blocks = new Block(pos());
				trim(EOF);
				while(f->peek()!='}'){
					Statement* e = getNextStatement(data.getModule(module));
					if(e!=NULL && e->getToken()!=T_VOID) blocks->values.push_back(e);
					trim(EOF);
					while(!f->done && f->peek()==';'){f->read();trim(data);}
					trim(EOF);
				}
				trim(data);
				if(paren && f->read()!='}') f->error("Need '}' for ending block statement");
				return blocks;
			}
			else{
				Statement* s = getNextStatement(data.getModule(module)/*.getLoc(PARSE_EXPR)*/);//todo check
				if(!f->done && f->peek()==';'){f->read();trim(data);}
				trim(data);
				return s;
			}
		}
		Statement* getNextType(ParseData data){
			trim(EOF);
			if(f->done || !isStartType(f->peek())) f->error("Could not find alphanumeric start for type parsing, found "+String(1,f->peek()));
			char tc = f->peek();
			if(!isStartName(tc)){
				if(tc=='('){
					f->read();
					char has  = 0;
					std::vector<std::pair<Statement*,String> > cp;
					std::vector<Statement*> cp1;
					f->trim(EOF);
					while(f->peek()!=')'){
						Statement* s = getNextType(data.getEndWith(EOF));
						f->trim(EOF);
						String st = "";
						if(f->peek()==':'){
							f->read();
							st = getNextName(EOF);
							f->trim(EOF);
							if(has==0 || has==1) has = 1;
							else f->error("Cannot have partially named tuple");
						} else has = 2;
						if(has==1) cp.push_back(std::pair<Statement*,String>(s,st));
						else cp1.push_back(s);
						if(f->peek()==','){
							f->read();
							f->trim(EOF);
						}
					}
					f->read();
					if(has==1){
						return new E_NAMED_TUPLE(pos(), cp);
					} else {
						return new E_TUPLE(pos(), cp1);
					}
				} else if(tc=='{'){
					f->read();
					f->error("Cannot parse map/set types yet");
				} else {
					f->read();
					assert(tc=='[');
					f->error("Cannot parse variable-length array types yet");
				}
			}
			Statement* currentType = getNextVariable(data);
			do{
				trim(data);
				auto marker = f->getMarker();
				bool typeOperation = f->getNextOperator(data.endWith)==".";
				if(!typeOperation){
					f->undoMarker(marker);
					break;
				}
				trim(data);
				if(f->done)  f->error("Could not find alphanumeric start for type parsing -ended");
				char c = f->peek();
				if(!isStartName(c)) f->error("Could not find alphanumeric start for type parsing");
				auto nextVariable = getNextName(data.endWith);
				currentType = new E_LOOKUP(pos(), currentType, nextVariable);
				f->trim(data.endWith);
				c = f->peek();
				while(c=='<'||c=='['){
					if(c=='<') f->error("Generic type parsing not implemented yet");
					else{
						f->error("Fixed-length Array type parsing not implemented yet");
					}
				}
			}while(true);
			return currentType;
		}
		Statement* getNextStatement(ParseData parse);
		Statement* getIfStatement(ParseData data,bool read=false){
			if(!read && f->getNextName(data.endWith)!="if") f->error("Could not find 'if' for if statement");
			//if(
			f->trim(EOF);
			//) f->error("Uncompleted if");
			std::vector<std::pair<Statement*,Statement* >> statements;
			Statement* c = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
			if(c->getToken()==T_VOID) f->error("Need condition for if");
			if(!f->done && f->peek()==':') f->read();
			Statement* s = getNextBlock(data.getLoc(PARSE_LOCAL));
			if(c->getToken()==T_VOID) f->error("Need expression for if");
			statements.push_back(std::pair<Statement*,Statement* >(c,s));
			trim(data);
			while(!f->done && f->peek()==';') f->read();
			auto marker = f->getMarker();
			String test = f->getNextName(data.endWith);
			Statement* finalElse = VOID;
			while(!f->done && (test=="else" || test=="elif")){
				f->trim(EOF);
				bool elif = test=="elif";
				if(!elif){
					auto m = f->getMarker();
					String yy = f->getNextName(data.endWith);
					if(yy=="if"){ elif=true; f->trim(EOF); }
					else { f->undoMarker(m); }
				}
				if(elif){
					c = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
					if(!f->done && f->peek()==':') f->read();
					s = getNextBlock(data.getLoc(PARSE_LOCAL));
					statements.push_back(std::pair<Statement*,Statement* >(c,s));
					trim(data.endWith);
					while(!f->done && f->peek()==';') f->read();
				}
				else{
					if(!f->done && f->peek()==':') f->read();
					f->trim(EOF);
					finalElse = getNextBlock(data);
					trim(data.endWith);
					while(!f->done && f->peek()==';') f->read();
				}
				marker = f->getMarker();
				test = f->getNextName(data.endWith);
			}
			f->undoMarker(marker);
			Statement* building = finalElse;
			for(unsigned int i = statements.size()-1; ; i--){
				building = new IfStatement(pos(), statements[i].first, statements[i].second, building);
				if(i==0) break;
			}
			return building;
		}
		Statement* getForLoop(ParseData data, bool read=false){
			if(!read && f->getNextName(data.endWith)!="for") f->error("Could not find 'for' for for-loop");
			//if(
			f->trim(EOF);//) f->error("Uncompleted for",true);
			bool paren = f->peek()=='(';
			//Standard for(i = 0; i<7; i++)
			OModule* module = new OModule(data.mod);
			if(paren){
				f->read();
				f->trim(EOF);
				Statement* init = VOID;
				if(f->peek()!=';') init = getNextStatement(ParseData(EOF, module, true, PARSE_LOCAL));
				if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
				f->trim(EOF);
				Statement* cond = VOID;
				if(f->peek()!=';') cond = getNextStatement(ParseData(EOF, module, true, PARSE_EXPR));
				if(cond->getToken()==T_VOID) cond = new obool(pos(), true);
				if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
				f->trim(EOF);
				Statement* inc = VOID;
				if(f->peek()!=')') inc = getNextStatement(ParseData(EOF, module, true, PARSE_EXPR));
				f->trim(EOF);
				if(f->read()!=')') f->error("Invalid additional piece of for loop",true);
				Statement* blocks = getNextBlock(data.getModule(module));
				return new ForLoop(pos(), init,cond,inc,blocks);
				//TODO implement for loop naming
			}
			else{
				String iterName = getNextName(EOF);
				f->trim(EOF);
				bool as = f->getNextName(data.endWith)=="in";
				f->trim(EOF);
				bool col = false;
				if(!f->done && f->peek()==':'){
					f->read(); f->trim(EOF);
				}
				if(as==col){
					f->error("Need either ':' or 'in' to separate iterator variable from iterable");
				}
				Statement* iterable = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
				f->trim(EOF);
				if(!f->done && f->peek()==':'){
					f->read();
					f->trim(EOF);
				}
				OModule* nmod = new OModule(data.mod);
				E_VAR* variable = new E_VAR(pos(), nmod->addPointer(pos(), iterName,DATA::getNull()));
				Statement* blocks = getNextBlock(ParseData(data.endWith, nmod, true,PARSE_LOCAL));
				//return new ForEachLoop(new E_VAR(module->addPointer(iterName,NULL,NULL,NULL,NULL,NULL)),iterable,blocks,"");
				//TODO implement for loop naming
				return new ForEachLoop(pos(),variable, iterable, blocks);
			}
		}
		Statement* getWhileLoop(ParseData data, bool read=false){
			if(!read && f->getNextName(data.endWith)!="while") f->error("Could not find 'while' for while-loop");
			//if(
			f->trim(EOF);//) f->error("Uncompleted while",true);
			bool paren = f->peek()=='(';
			if(paren) f->read();
			Statement* cond = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
			if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of while",true);
			if(f->trim(EOF)) f->error("Uncompleted while",true);
			if(f->peek()==':') f->read();
			return new IfStatement(pos(), cond, new DoWhileLoop(pos(), cond,getNextBlock(data)),VOID);
			//TODO implement while loop naming
		}
		Statement* getDoLoop(ParseData data, bool read=false){
			if(!read && f->getNextName(data.endWith)!="do") f->error("Could not find 'do' for do-while-loop");
			//if(
			f->trim(EOF);//) f->error("Uncompleted do-while",true);
			if(f->peek()==':') f->read();
			if(f->trim(EOF)) f->error("Uncompleted do-while",true);
			Statement* blocks = getNextBlock(data);
			f->trim(EOF);
			if(f->getNextName(EOF)!="while") f->error("Must complete 'while' part of do{...}while",true);
			bool paren = f->peek()=='(';
			if(paren) f->read();
			Statement* cond = getNextStatement(ParseData((paren)?EOF:data.endWith, data.mod, true,PARSE_EXPR));
			if(paren && f->read()!=')') f->error("Need terminating ')' for conditional of do-while",true);
			if(!f->done && f->peek()==';') f->read();
			return new DoWhileLoop(pos(), cond, blocks);
			//TODO implement do loop naming
		}
		Statement* getLambdaFunction(ParseData data, bool read=false){
			if(!read && f->getNextName(data.endWith)!="do") f->error("Could not find 'do' for do-while-loop");
			if(f->trim(EOF)) f->error("Uncompleted lambda function");
			OModule* module = new OModule(data.mod);
			std::vector<Declaration*> arguments;
			if(!f->done) arguments = parseArguments(ParseData(data.endWith, module, true,PARSE_LOCAL),':');
			if(f->trim(EOF)) f->error("Lambda Function without body");
			if(!f->done && f->peek()==':'){
				f->read();
				if(f->trim(EOF)) f->error("Lambda Function without body (c)");
			}
			Statement* methodBody = getNextBlock(ParseData(data.endWith, module,true,PARSE_LOCAL));
			return new lambdaFunction(pos(), arguments, methodBody, rdata);
		}
		Statement* getFunction(ParseData data, String temp="", oclass* outer=NULL){
			if(temp=="") temp = f->getNextName(EOF);
			trim(EOF);
			if (temp == "def" || temp=="gen"){
				auto mark = f->getMarker();
				Statement* returnName = getNextType(data);
				f->trim(EOF);
				{
					auto nex = f->peek();
					if(!isStartName(nex)){
						f->undoMarker(mark);
						returnName = new ClassProtoWrapper(autoClass);
					}
				}

				OModule* module = new OModule(data.mod);
				std::vector<std::pair<String,PositionID> > methodName;
				bool isOperator = false;
				while(isStartName(f->peek())){
					String method = getNextName(EOF);
					if(method.length()==0) break;
					f->trim(EOF);
					auto mark = f->getMarker();
					bool op = f->getNextOperator(EOF)==".";
					if(!op) f->undoMarker(mark);
					f->trim(EOF);
					if(method=="operator"){
						String t = f->getNextOperator(data.endWith);
						while(f->peek()=='[' || f->peek()=='%' || f->peek()==']') t+=String(1,f->read())+f->getNextOperator(data.endWith);
						method = "~"+t;
						f->trim(EOF);
						isOperator = true;
					}
					methodName.push_back(std::pair<String,PositionID>(method,pos()));
					if(isOperator || !op) break;
				}
				std::vector<Declaration*> arguments;
				if(!f->done){
					if(f->peek()=='('){
						f->read();
						arguments = parseArguments(ParseData(EOF, module,true,PARSE_LOCAL),')');
					}
					else f->error("Need '(' in function declaration, found "+String(1,f->read()),true);
				}
				trim(data);
				if(!f->done && f->peek()==':'){
					f->read();
					trim(data);
				}
				bool paren;
				Statement* funcName;
				Statement* func;
				if(methodName.size()<=1 && outer==NULL){
					if(methodName.size()==0) funcName = NULL;
					else if(methodName.size()==1){
						if(isOperator){
							funcName = new E_VAR(pos(), new ReferenceElement("",NULL, methodName[0].first,DATA::getFunction(NULL,NULL),funcMap()));
						} else {
							funcName = new E_VAR(pos(), data.mod->getFuncPointer(pos(), methodName[0].first));
						}
					}
					Statement* methodBody = getNextBlock(ParseData(data.endWith, module, true,PARSE_LOCAL),&paren);
					auto tmp = pos();
					if(temp=="gen")
						func = new E_GEN(tmp, funcName, returnName, arguments, methodBody,rdata);
					else
						func = new userFunction(tmp, funcName, returnName, arguments, methodBody,rdata);
				}
				else {
					//TODO change scope
					//myMod = NULL;//make combined scope
					if(outer==NULL){
						funcName = new E_VAR(pos(), new LateResolve(data.mod, methodName[0].first, pos()));
						for(unsigned int i = 1; i+1<methodName.size(); i++)
							funcName = new E_LOOKUP(methodName[i].second, funcName, methodName[i].first);
					} else{
						funcName = outer;//new E_LOOKUP(methodName[0].second, outer, methodName[0].first);
						for(unsigned int i = 0; i+1<methodName.size(); ++i){
							funcName = new E_LOOKUP(methodName[i].second, funcName, methodName[i].first);
						}
					}
					ReferenceElement* thisPointer = module->addPointer(pos(),"this",DATA::getNull());
					Statement* methodBody = getNextBlock(ParseData(data.endWith, module,true,PARSE_LOCAL), &paren);
					PositionID tmp = pos();
					if((outer!=NULL)?(methodName.back().first == outer->name):(methodName.back().first==methodName[methodName.size()-2].first)){
						func = new constructorFunction(tmp, thisPointer, funcName, arguments, methodBody,rdata);
					}
					else{
						if(temp=="gen"){
							func = new E_GEN(tmp, funcName, returnName, arguments, methodBody,rdata, methodName.back().first,thisPointer);
						}
						else
							func = new classFunction(tmp, thisPointer, methodName[methodName.size()-1].first, funcName, returnName, arguments, methodBody,rdata);

					}
				}
				trim(data);
				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				trim(data);
				if(data.operatorCheck && !semi) return operatorCheck(data, func);
				return func;
			}
			else if (temp == "extern"){
				Statement* retV = getNextType(ParseData(EOF, data.mod,true,data.loc));
				trim(data);
				String methodName = getNextName(EOF);
				E_VAR* externName = new E_VAR(pos(), data.mod->getFuncPointer(pos(), methodName));
				externName->getMetadata(rdata)->setObject(DATA::getFunction(NULL,NULL));
				trim(data);
				if(f->peek()!='('){
					f->error("'(' required after extern not "+String(1,f->peek()),true);
				}
				f->read();
				std::vector<Declaration*> dec;
				OModule* m2 = new OModule(data.mod);
				while(true){
					trim(data);
					if(f->peek()==')') break;
					Declaration* d= getNextDeclaration(ParseData(EOF, m2, true,PARSE_LOCAL));
					dec.push_back(d);
					trim(data);
					if(f->peek()==',') f->read();
				}
				if(f->read()!=')') f->error("Need ending ')' for extern", true);
				return new externFunction(pos(), externName, retV, dec,rdata);
			}
			else{
				f->error("Invalid function start -- used "+temp);
				return NULL;
			}

		}
		Statement* getClass(ParseData data, bool read=false,oclass* outer=NULL){
			if(!read && f->getNextName(data.endWith)!="class") f->error("Could not find 'class' for class declaration");
			String name = getNextName(EOF);//TODO -- allow generic class definition
			LayoutType primitive;
			if(name=="primitive" || name=="primitive_pointer"){
				primitive = (name=="primitive")?PRIMITIVE_LAYOUT:PRIMITIVEPOINTER_LAYOUT;
				name = getNextName(EOF);//TODO -- allow generic class definition (here as well)
			} else primitive = POINTER_LAYOUT;
			if(f->trim(EOF)) f->error("No class defined!");
			Statement* superClass=NULL;
			if(f->peek()==':'){
				f->read();
				superClass = getNextType(data.getEndWith(EOF));
				f->trim(EOF);
			}
			if(f->read()!='{') f->error("Need opening brace for class definition");
			f->trim(EOF);
			ReferenceElement* re = data.mod->getClassPointer(pos(), name);
			OModule* classMod = new OModule(data.mod);
			oclass* selfClass = new oclass(pos(), name, superClass, re, primitive, outer);
			while(f->peek()==';'){
				f->read();
				f->trim(EOF);
			}
			while(f->peek()!='}'){
				auto mark = f->getMarker();
				String temp = f->getNextName(EOF);
				bool stat = false;
				if(temp=="static"){
					stat = true;
					f->trim(EOF);
					mark = f->getMarker();
					temp = f->getNextName(EOF);
				}
				ParseData nd = ParseData(EOF, classMod, true, (stat)?data.loc:PARSE_LOCAL);
				if(temp=="class"){
					if(stat!=true) pos().error("Optricks does not currently support non-static classes");
					Statement* uClass = getClass(nd, true, selfClass);
					selfClass->under.push_back(uClass);
				} else if(temp=="def" || temp=="gen"){
					Statement* func = getFunction(nd, temp,selfClass);//TODO allow methods to be static
					selfClass->under.push_back(func);
				} else if(temp=="extern"){
					pos().error("Optricks does not allow the use of external functions in the definition classes!");
				} else{
					f->undoMarker(mark);
					Declaration* dec = getNextDeclaration(nd);
					selfClass->data.push_back(dec);
				}
				f->trim(EOF);
				while(f->peek()==';'){
					f->read();
					f->trim(EOF);
				}
			}
			if(f->read()!='}') f->error("Need closing brace for class definition");
			return selfClass;
		}
		Statement* operatorCheck(ParseData data, Statement* exp){
			if(f->done || f->trim(data.endWith))	return exp;
			if(f->last()==data.endWith){
				//f->write(data.endWith);
				return exp;
			}
			char tchar = f->peek();
			if(tchar=='{') return exp;
			if(tchar=='['){
				f->read();
				std::vector<Statement*> stack;
				while(true){
					bool done = true;
					while(!f->done){
						if(f->trim(data.endWith)) f->error("Indexed operator check hit EOF",true);
						char t = f->peek();
						if(t==',' || t==':'){
							f->read();
							if(t==',') exp = getIndex(f, exp,stack);
							else stack.push_back(NULL);
						}
						else if(t==']'){
							done = false;
							break;
						}
						else break;
					}
					if(f->trim(data.endWith)) f->error("Uncompleted '[' index",true);
					if(!done) break;
					stack.push_back(getNextStatement(data.getLoc(PARSE_EXPR)));
				}
				if(f->done)	f->error("Uncompleted '[' array 2",true);
				char te;
				if((te = f->read())!=']') f->error("Cannot end '[' array with "+str<char>(te),true);
				Statement* nex = getIndex(f, exp, stack);
				f->trim(data.endWith);
				auto mark = f->getMarker();
				if(f->read()=='=' && f->peek()!='='){
					//TODO ternary operator #[#] = #
					cerr << "TODO ternary operator #[#] = #" << endl << flush;
					exit(1);
				} else f->undoMarker(mark);
				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(data.endWith);
				if(!semi) return operatorCheck(data, nex);
				else return nex;
			}
			else if(tchar=='('){
				//TODO parse function args,  cannot do getNextStatement
				// due to operatorCheck on tuple
				Statement* e = getNextStatement(ParseData(data.endWith, data.mod, false,PARSE_EXPR));
				Statement* ret;
				if(e->getToken()==T_PARENS)
					ret = new E_FUNC_CALL(pos(), exp, std::vector<Statement*>(1,
							((E_PARENS*)e)->inner));
				else ret = new E_FUNC_CALL(pos(), exp, ((E_TUPLE*)e)->values);

				f->trim(data.endWith);

				bool semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				f->trim(data.endWith);
				if(!semi) return operatorCheck(data, ret);
				else return ret;
			}
			else if(tchar=='{'){
				f->read();
				f->trim(data.endWith);
				cerr << " '{' operatorCheck not implemented yet" << endl << flush;
				exit(0);
			}
			else if (tchar=='?'){
				f->read();
				f->trim(data.endWith);
				Statement* op1 = getNextStatement(ParseData(EOF, data.mod, true, PARSE_EXPR));

				f->trim(data.endWith);
				if(f->read()!=':') f->error("Ternary operator requires ':'",true);
				Statement* op2 = getNextStatement(ParseData(EOF, data.mod, true, PARSE_EXPR));
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
			String tmp = f->getNextOperator(data.endWith);

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
			else if (tmp=="."){
				String name = getNextName(data.endWith);
				if(name.length()==0) f->error("Name for lookup cannot be "+name);
				fixed = new E_LOOKUP(pos(), exp, name);
			}
			else if(tmp=="="){
				Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
				fixed = new E_SET(pos(), exp, post);
			}
			else if(tmp=="++" || tmp=="--"){
				fixed = new E_POSTOP(pos(), tmp, exp);
			}
			else if(tmp.size()>=2 && tmp[tmp.size()-1]=='=' && !(tmp[tmp.size()-2]=='=' || tmp[tmp.size()-2]=='!' || (tmp.size()==2 && (tmp[0]=='<' || tmp[0]=='>')))){
				tmp = tmp.substr(0,tmp.size()-1);
				Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
				fixed = new E_SET(pos(), exp, new E_BINOP(pos(), exp, post, tmp));
			}
			else{
				Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
				if(post->getToken()==T_VOID)
					fixed = new E_POSTOP(pos(), tmp, exp);
				else fixed = (new E_BINOP(pos(), exp, post,tmp))->fixOrderOfOperations();
			}
			f->trim(data.endWith);
			bool semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			f->trim(data.endWith);
			if(!semi) fixed = operatorCheck(data, fixed);
			return fixed;
		}
};

Statement* Lexer::getNextStatement(ParseData data){
	if(f->done || f->trim(EOF)) return VOID;
	int nex = f->peek();
	if(f->done || nex==EOF || nex==','||nex==')' ||nex==']'/*|| nex==endWith*/) return VOID;
	if(nex==';') return VOID;
	bool semi;
	if (isStartName(nex)) {
		auto undoRead = f->getMarker();
		String temp = f->getNextName(data.endWith);
		if(temp=="class") return getClass(data, true);
		else if(temp=="if") return getIfStatement(data,true);
		else if(temp=="for") return getForLoop(data,true);
		else if(temp=="while") return getWhileLoop(data,true);
		else if(temp=="do") return getDoLoop(data,true);
		else if(temp=="lambda") return getLambdaFunction(data,true);
		else if(temp=="gen" || temp=="def" || temp=="extern") return getFunction(data,temp);
		else if(temp=="import"){
			trim(data);
			String tI = f->readString(data.endWith);
			if(tI.length()==0) f->error("Could not import file of no length");
			return new ImportStatement(pos(),tI);
		}
		else if (temp=="true" || temp=="false"){
			Statement* te = new obool(pos(), temp=="true");
			trim(data);
			semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			trim(data);
			if(data.operatorCheck && !semi) return operatorCheck(data, te);
			else return te;
		}
		else if(temp=="return" || temp=="yield"){
			//			if(!data.allowsDec()) f->error("Cannot have return here");
			trim(data);
			Statement* t = VOID;
			//TODO prevent returns from use in iterator
			if(f->peek()!='}' && f->peek()!=',' && f->peek()!=':' && f->peek()!=')') t = getNextStatement(data.getLoc(PARSE_EXPR));
			return new E_RETURN(pos(), t, "", (temp=="return")?RETURN:YIELD);
		}
		else if(temp=="break" || temp=="continue"){
			//			if(!data.allowsDec()) f->error("Cannot have return here");
			trim(data.endWith);
			String name = (isStartName(f->peek()))?(getNextName(EOF)):"";
			return new E_RETURN(pos(), NULL, name, (temp=="break")?BREAK:CONTINUE );
		}
		else{
			auto start = f->getMarker();
			trim(data);
			if(data.allowsDec() && (!f->interactive || f->last()!='\n') && start!=f->getMarker() && isStartName(f->peek()) ){
				f->undoMarker(undoRead);
				return getNextDeclaration(data);
			} else {
				f->undoMarker(undoRead);
				E_VAR* nextVar = getNextVariable(data);
				semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				trim(data);
				if(data.operatorCheck && !semi)
					return operatorCheck(data, nextVar);
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
				oobject* num = f->readNumber(data.endWith);
				trim(data);
				semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				trim(data);
				if(data.operatorCheck && !semi) return operatorCheck(data, num);
				else return num;
			}
			case '\'':
			case '"':{
				oobject* str;
				String tmp = f->readString(data.endWith);
				if(tmp.length()==1) str = new ochar(pos(), tmp[0]);
				else str = new ostring(pos(), tmp);
				trim(data);
				semi  = false;
				if(!f->done && f->peek()==';'){ semi = true; }
				trim(data);
				if(data.operatorCheck && !semi) return operatorCheck(data, str);
				else return str;
			}
			case '{':
			case '[':
			case '(':{
				auto undoRead = f->getMarker();
				char open = f->read();
				char close = (open=='{')?'}':((open=='[')?']':')');
				trim(data);
				//E_ARR* arr = new E_ARR(pos());
				std::vector<Statement*> values;
				std::vector<Statement*> seconds;
				Statement* temp;
				char te;
				if(f->peek()==close) f->read();
				else{
					temp = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
					forceAr = false;
					while(temp->getToken()!=T_EOF){
						values.push_back(temp);
						f->trim(EOF);
						if(f->peek()==':'){
							f->read();
							seconds.push_back(getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR)));
							f->trim(EOF);
						}
						bool comma = true;
						if(!f->done && f->peek()==','){
							f->read();
							forceAr = true;
						}
						else comma = false;
						if(f->trim(data.endWith)) f->error("Uncompleted inline array",true);

						if(f->peek()==close) break;
						if(!comma){
							f->error("Missing , in inline array or wrong end char",true);
						}
						if(f->trim(data.endWith)) f->error("Uncompleted '(' array",true);
						temp = getNextStatement(ParseData(EOF, data.mod, true,PARSE_EXPR));
					}
					if(open=='(' && !forceAr && values.size()==1){
						Statement* temp = values[0];
						//delete arr;
						if((te = f->read())!=close) f->error("Cannot end inline paren with "+
								String(1,te)+" instead of "+String(1,close)
						);
						temp = new E_PARENS(pos(), temp);
						trim(data);
						semi  = false;
						if(!f->done && f->peek()==';'){ semi = true; }
						trim(data);
						if(data.operatorCheck && !semi) return operatorCheck(data, temp);
						else return temp;
					}
					if(f->done) f->error("Uncompleted inline 2");
					if((te = f->read())!=close) f->error("Cannot end inline array with "+
							String(1,te)+" instead of "+String(1,close));
				}
				//todo allow e_map
				if(open=='(' && seconds.size()>0 && values.size()!=seconds.size())
					f->error("Cannot have partially-named tuple",true);
				Statement* arr = (open=='(')?((Statement*)(new E_TUPLE(pos(), values))):(
						(open=='[')?((Statement*)(new E_ARR(pos(), values))):
								((Statement*)(new E_ARR(pos(), values))) //todo change to E_SET
				);



				auto start = f->getMarker();
				trim(data);
				if(data.allowsDec() && (!f->interactive || f->last()!='\n') && start!=f->getMarker() && isStartName(f->peek()) ){
					f->undoMarker(undoRead);
					return getNextDeclaration(data);
				} else {
					trim(data);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					trim(data);
					if(data.operatorCheck && !semi) return operatorCheck(data, arr);
					else return arr;
				}
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
					Statement* toReturn = f->readNumber(data.endWith);
					trim(data);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					trim(data);
					if(data.operatorCheck && !semi)
						toReturn = operatorCheck(data, toReturn);
					return toReturn;
				}
				else{
					String temp(1,n);
					while(in<String>(PRE_OPERATORS, temp+String(1,f->peek()))){
						temp+=String(1,f->read());
					}
					Statement* toReturn = new E_PREOP(pos(), temp,getNextStatement(data.getLoc(PARSE_EXPR)));
					trim(data);
					semi  = false;
					if(!f->done && f->peek()==';'){ semi = true; }
					trim(data);
					if(data.operatorCheck && !semi)
						toReturn = operatorCheck(data, toReturn);
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
#endif /* LEXER_HPP_ */
