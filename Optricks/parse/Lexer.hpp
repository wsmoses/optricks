/*
 * Lexer.hpp
 *
 *  Created on: Aug 6, 2013
 *      Author: wmoses
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include "./Stream.hpp"
#include "../ast/Declaration.hpp"
#include "../ast/DoWhileLoop.hpp"
#include "../ast/WhileLoop.hpp"
#include "../ast/ForLoop.hpp"
#include "../ast/ForEachLoop.hpp"
#include "../ast/IfStatement.hpp"
#include "../ast/Block.hpp"
#include "../ast/ImportStatement.hpp"
#include "../ast/E_ARR.hpp"
#include "../ast/E_RETURN.hpp"
#include "../ast/E_BINOP.hpp"
#include "../ast/E_LOOKUP.hpp"
#include "../ast/E_UOP.hpp"
#include "../ast/E_VAR.hpp"
#include "../ast/E_SET.hpp"
#include "../ast/E_TUPLE.hpp"
#include "../ast/E_PARENS.hpp"
#include "../ast/E_TERNARY.hpp"
#include "../ast/E_PARENS.hpp"
#include "../ast/E_FUNC_CALL.hpp"
#include "../ast/OClass.hpp"
#include "../ast/function/E_GEN.hpp"
#include "../ast/function/ExternFunction.hpp"
#include "../ast/function/UserFunction.hpp"
#include "../ast/function/ClassFunction.hpp"
#include "../ast/function/LambdaFunction.hpp"
#include "../ast/function/ConstructorFunction.hpp"
#include "../language/data/literal/StringLiteral.hpp"
#include "../language/data/DeclarationData.hpp"
#include "../language/class/builtin/CharClass.hpp"
#include "../language/class/EnumClass.hpp"

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

#define LEXER_C_
class Lexer{
public:
	Stream* f;
	OModule* myMod;
	//char endWith;
	//	bool inter;
	Lexer(Stream* t, bool inter):f(t){
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
			if(getcwd(cwd,sizeof(cwd))==nullptr) PositionID(0,0,fileName).error("Could not determine Current Working Directory");
			String dir, file;
			getDir(fileName, dir, file);
			if(dir!="." && chdir(dir.c_str())!=0) PositionID(0,0,fileName).error("Could not change directory to "+dir+"/"+file);
			//cout << "Opened: " << dir << "/" << file << endl << flush;
			Stream* tmp = f;
			Stream next(file,false);
			f = &next;
			while(true){
				while(f->peek()==';') f->read();
				Statement* s = getNextStatement(EOF,global);
				if(s==nullptr || s->getToken()==T_VOID) break;
				if(s->getToken()==T_IMPORT){
					ImportStatement* import = (ImportStatement*)s;
					std::vector<String> pl = {import->toImport};
					getStatements(global, debug, pl, stats);
					continue;
				}
				if(debug && s->getToken()!=T_VOID){
					std::cout << s << ";" << endl << endl << flush;
				}
				stats.push_back(s);
			}
			f = tmp;
			if(chdir(cwd)!=0) pos().error("Could not change directory back to "+String(cwd));
		}
	}
	void execFiles(bool global, std::vector<String> fileNames, llvm::raw_ostream* file, bool debug, int toFile=0,unsigned int optLevel = 3){
		std::vector<Statement*> stats;
		getStatements(global, debug, fileNames, stats);
		for(auto& n: stats) n->registerClasses();
		for(auto& n: stats){
			n->registerFunctionPrototype(getRData());
		}
		for(auto& n: stats){
			n->buildFunction(getRData());
		}

		llvm::FunctionType* FT = llvm::FunctionType::get(VOIDTYPE, llvm::SmallVector<llvm::Type*,0>(0), false);
		assert(FT);
		llvm::Function* F = getRData().CreateFunction("main",FT,EXTERN_FUNC);
		assert(F);
		llvm::BasicBlock* BB = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", F);
		getRData().builder.SetInsertPoint(BB);
		for(auto& n: stats) n->evaluate(getRData());
		getRData().builder.CreateRetVoid();
		getRData().FinalizeFunction(F,debug);
		if(debug){
			this->myMod->write(cerr);
			getRData().lmod->dump();
			cerr << endl << flush;
		}
		if(toFile>0)
			llvm::verifyModule(* getRData().lmod);
		//auto modOpt = new PassManager();
		//FunctionPassManager* fnOpt = new FunctionPassManager(getRData().lmod);

		// Set up optimisers
		//PassManagerBuilder pmb;
		//pmb.OptLevel = optLevel;
		//pmb.populateFunctionPassManager(*fnOpt);
		//if(toFile>0) pmb.populateModulePassManager(*modOpt);
		//fnOpt->run(*F);
		//if(toFile>0) modOpt->run(*(getRData().lmod));
		if(toFile>0) getRData().mpm.run(* getRData().lmod);

		if(debug){
			//getRData().lmod->dump();
		}
		if(toFile==3){
			//				llvm::raw_os_ostream raw_stream(file);
			assert(file);
			llvm::WriteBitcodeToFile(getRData().lmod, *file);
			//				getRData().lmod->print(raw_stream, 0);
		} else if(toFile==2){
			//				llvm::raw_os_ostream raw_stream(file);
			assert(file);
			getRData().lmod->print(*file,0);
			//				WriteBitcodeToFile(getRData().lmod, file);
			//				getRData().lmod->print(raw_stream, 0);
		} else {
			assert(getRData().exec);
			void *FPtr = getRData().exec->getPointerToFunction(F);
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
	E_LOOKUP* getLookup(ParseData data, Statement* v, String name){
		if(name.length()==0) f->error("Name for lookup cannot be "+name);
		f->trim(data.endWith);
		E_LOOKUP* fixed = new E_LOOKUP(pos(), v, name,f->peek()=='{');
		if(f->peek()=='{'){
			f->read();
			f->trim(EOF);
			while(true){
				auto z = f->peek();
				if(z==EOF){
					pos().error("Unclosed '{' for template", false);
					break;
				}
				else if(z=='}'){
					f->read();
					break;
				} else if(z==','){
					f->read();
					f->trim(EOF);
				}
				//STUFF
				((E_LOOKUP*)fixed)->t_args.add(getNextStatement(data.getEndWith(EOF)));
				f->trim(EOF);
			}
		}
		return fixed;
	}
	E_VAR* getNextVariable(ParseData data, bool allowsAuto, bool allowsTemplate){
		auto tmp = getNextName(data.endWith);
		if(tmp=="auto"){
			if(allowsAuto) return nullptr;
			else{
				pos().error("Cannot use 'auto' has variable -- is a keyword");
				assert(0);
				exit(1);
			}
		}
		auto pek = f->peek();
		E_VAR* fixed = new E_VAR(Resolvable(data.mod,tmp,pos()),pek=='{');
		if(pek=='{'){
			f->read();
			f->trim(EOF);
			while(true){
				auto z = f->peek();
				if(z==EOF){
					pos().error("Unclosed '{' for template", false);
					break;
				}
				else if(z=='}'){
					f->read();
					break;
				} else if(z==','){
					f->read();
					f->trim(EOF);
				}
				fixed->t_args.add(getNextStatement(data.getEndWith(EOF)));
				f->trim(EOF);
			}
		}
		return fixed;
		//Resolvable pointer;
		//if(late) pointer = new;
		//else pointer = &data.mod->addPointer(pos(), getNextName(data.endWith),DATA::getNull());
		//return new E_VAR(pos(), late?(Resolvable(data.mod, getNextName(data.endWith), pos())):());
	}
	Declaration* getNextDeclaration(ParseData data,bool global=false,bool allowAuto=false,bool enableScope=true,bool emptyName=false){
		trim(data);
		Statement* declarationType = getNextType(data.getEndWith(EOF),true);
		trim(EOF);
		String varName;
		if(allowAuto && (declarationType->getToken()==T_VAR) && !isStartName(f->peek())){
			varName = ((E_VAR*)(declarationType))->pointer.name;
			declarationType = nullptr; // now means auto
		} else{
			if(emptyName && !isStartName(f->peek()))
				varName = "";
			else
				varName = getNextName(data.endWith);
		}
		trim(data);
		Statement* value = nullptr;
		if(f->peek()=='='){
			f->read();
			value = getNextStatement(data.getLoc(PARSE_EXPR));
			assert(value);
		}
		assert(data.mod);
		auto RT=new Declaration(pos(), declarationType, E_VAR(Resolvable(data.mod, varName, pos()),false), global || (data.loc==PARSE_GLOBAL), value);
		if(enableScope && varName.size() > 0)
			data.mod->addVariable(pos(), varName,new DeclarationData(RT));
		assert(RT);
		return RT;
	}
	Statement* getNextStatement(char endWith,bool global){
		return getNextStatement(ParseData(endWith,myMod,true,global?PARSE_GLOBAL:PARSE_LOCAL));
	}
	void parseArguments(std::vector<Declaration*>& args, ParseData data, char finish=')'){
		assert(args.size()==0);
		if(f->done) return;
		while(true){
			trim(data);
			if(f->peek()==finish){
				f->read();
				return;
			}
			Declaration* d = getNextDeclaration(data.getEndWith(EOF),false,true);
			for(const Declaration*const& a:args){
				if(d->variable.pointer.name == a->variable.pointer.name){
					f->error("Cannot have duplicate argument name: "+a->variable.pointer.name, true);
				}
			}
			assert(d);
			args.push_back(d);
			trim(data);
			char tchar = f->peek();
			if(tchar==finish || tchar==','){
				f->read();
				if(tchar==finish) return;
			}
			else f->error("Could not parse arguments - encountered character "+String(1,tchar));
		}
		return;
	}
	inline PositionID pos() const{
		assert(f);
		return f->pos();
	}
	inline void trim(char c){
		//		if(
		assert(f!=nullptr);
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
	Statement* getNextBlock(ParseData data, bool*par=nullptr){
		f->trim(EOF);
		bool paren = f->peek()=='{';
		if(paren) f->read();
		if(par!=nullptr) *par=paren;
		if(paren){
			Block* blocks = new Block(pos(), data.mod);
			trim(EOF);
			while(f->peek()!='}'){
				Statement* e = getNextStatement(data.getModule(&(blocks->module)));
				if(e->getToken()==T_VOID){
					pos().error("Needed '}' to end block, found '"+String(1,f->peek())+"'");
					exit(1);
				}
				if(e!=nullptr && e->getToken()!=T_VOID) blocks->values.push_back(e);
				trim(EOF);
				while(!f->done && f->peek()==';'){f->read();trim(data);}
				trim(EOF);
			}
			trim(data);
			if(paren && f->read()!='}') f->error("Need '}' for ending block statement");
			return blocks;
		}
		else{
			Statement* s = getNextStatement(data/*.getLoc(PARSE_EXPR)*/);//todo check
			if(!f->done && f->peek()==';'){f->read();trim(data);}
			trim(data);
			return s;
		}
	}
	Statement* getNextType(ParseData data, bool allowsAuto){
		trim(EOF);
		if(f->done || !isStartType(f->peek())) f->error("Could not find alphanumeric start for type parsing, found "+String(1,f->peek()));
		char tc = f->peek();
		if(!isStartName(tc)){
			if(tc=='('){
				f->read();
				char has  = 0;
				std::vector<String> nam;
				std::vector<Statement*> cp1;
				f->trim(EOF);
				while(f->peek()!=')'){
					Statement* s = getNextType(data.getEndWith(EOF),true);
					if(!s) pos().error("Cannot have auto class inside of tuple");
					cp1.push_back(s);
					f->trim(EOF);
					String st = "";
					if(f->peek()==':'){
						f->read();
						st = getNextName(EOF);
						f->trim(EOF);
						if(has==0 || has==1) has = 1;
						else f->error("Cannot have partially named tuple");
					} else has = 2;
					if(has==1) nam.push_back(st);
					if(f->peek()==','){
						f->read();
						f->trim(EOF);
					}
				}
				f->read();
				if(has==1){
					return new E_NAMED_TUPLE(pos(), cp1, nam);
				} else {
					return new E_TUPLE(cp1);
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
		Statement* currentType = getNextVariable(data, allowsAuto, true);
		if(!currentType) return nullptr;
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
			currentType = getLookup(data, currentType, nextVariable);
			f->trim(data.endWith);
			c = f->peek();
			while(c=='<'||c=='['){
				if(c=='<') f->error("Generic type parsing not implemented yet");
				else{
					f->error("Fixed-length Array type parsing not implemented yet");
				}
			}
		}while(true);
		while(true){
			if(f->peek()=='&'){
				f->read();
				currentType = new E_UOP(pos(), "&", currentType,UOP_POST);
				f->trim(data.endWith);
				break;
			} else if(f->peek()=='%'){
				f->read();
				currentType = new E_UOP(pos(), "%", currentType,UOP_POST);
				f->trim(data.endWith);
				break;
			} else if(f->peek()!='[') break;
			auto mark = f->getMarker();
			f->read();
			f->trim(EOF);
			if(f->peek()==']'){
				f->read();
				currentType = new E_UOP(pos(), "[]", currentType,UOP_POST);
				f->trim(data.endWith);
			} else{
				f->undoMarker(mark);
				break;
			}
		}
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
		Statement* finalElse = nullptr;
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
		assert(building);
		return building;
	}
	Statement* getForLoop(ParseData data, bool read=false){
		if(!read && f->getNextName(data.endWith)!="for") f->error("Could not find 'for' for for-loop");
		//if(
		f->trim(EOF);//) f->error("Uncompleted for",true);
		bool paren = f->peek()=='(';
		//Standard for(int i = 0; i<7; i++)
		if(paren){
			f->read();
			f->trim(EOF);
			auto FORLOOP = new ForLoop(pos(),data.mod);

			if(f->peek()!=';') FORLOOP->initialize = getNextStatement(ParseData(EOF, & FORLOOP->module, true, PARSE_LOCAL));
			else FORLOOP->initialize = VOID_STATEMENT;
			if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
			f->trim(EOF);

			if(f->peek()!=';'){
				FORLOOP->condition = getNextStatement(ParseData(EOF, & FORLOOP->module, true, PARSE_EXPR));
				if(FORLOOP->condition->getToken()==T_VOID)
					FORLOOP->condition = ConstantData::getTrue();
			} else FORLOOP->condition = ConstantData::getTrue();
			if(!f->done && (f->peek()==';' || f->peek()==',')) f->read();
			f->trim(EOF);

			if(f->peek()!=')') FORLOOP->increment = getNextStatement(ParseData(EOF, & FORLOOP->module, true, PARSE_EXPR));
			else FORLOOP->increment = VOID_STATEMENT;
			f->trim(EOF);
			if(f->read()!=')') f->error("Invalid additional piece of for loop",true);
			FORLOOP->toLoop = getNextBlock(data.getModule(& FORLOOP->module));
			return FORLOOP;
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
			//TODO CLEANUP FOREACH MODULES
			OModule* nmod = new OModule(data.mod);
			nmod->addVariable(pos(), iterName,&VOID_DATA);
			E_VAR* variable = new E_VAR( Resolvable(nmod,iterName,pos()), false);
			Statement* blocks = getNextBlock(ParseData(data.endWith, nmod, true,PARSE_LOCAL));
			//return new ForEachLoop(new E_VAR(module->addPointer(iterName,nullptr,nullptr,nullptr,nullptr,nullptr)),iterable,blocks,"");
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
		return new WhileLoop(pos(),cond, getNextBlock(data));
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
		if(!read && f->getNextName(data.endWith)!="lambda") f->error("Could not find 'lambda' in lambda function");
		if(f->trim(EOF)) f->error("Uncompleted lambda function");
		auto LAMBDA = new LambdaFunction(pos(), data.mod);
		if(!f->done) parseArguments(LAMBDA->declaration, ParseData(EOF, & LAMBDA->module, true,PARSE_LOCAL),':');
		if(f->trim(EOF)) f->error("Lambda Function without body");
		if(!f->done && f->peek()==':'){
			f->read();
			if(f->trim(EOF)) f->error("Lambda Function without body (c)");
		}
		LAMBDA->methodBody = getNextBlock(ParseData(data.endWith, & LAMBDA->module,true,PARSE_LOCAL));
		return LAMBDA;
	}
	E_FUNCTION* getFunction(ParseData data, String temp="", AbstractClass* outer=nullptr,bool stat=false){
		if(temp=="") temp = f->getNextName(EOF);
		trim(EOF);
		if (temp == "def" || temp=="gen" || temp=="inl"){
			auto mark = f->getMarker();
			bool staticF = stat;
			if(f->getNextName(EOF)=="static"){
				staticF = true;
				trim(EOF);
				mark = f->getMarker();
			} else f->undoMarker(mark);
			auto returnNameTemp = getNextType(data,true);
			f->trim(EOF);
			if(returnNameTemp){
				auto nex = f->peek();
				if(!isStartName(nex)){
					f->undoMarker(mark);
					returnNameTemp = nullptr;
				}
			}

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
			E_FUNCTION* func;
			if(methodName.size()<=1 && outer==nullptr){
				String name;
				if(methodName.size()==0) name = "";
				else{
					assert(methodName.size()==1);
					if(isOperator){
						pos().error("Operator overloading not implemented yet");
						exit(1);
						assert(methodName[0].first.length()>0);
						name = ":"+methodName[0].first;
					} else {
						data.mod->addFunction(pos(), methodName[0].first);
						assert(methodName[0].first.length()>0);
						name = methodName[0].first;
					}
				}
				if(temp=="gen")
					func = new E_GEN(pos(), data.mod,name);//arguments, funcName, returnName, methodBody);
				else
					func = new UserFunction(pos(), data.mod,name);//arguments, funcName, returnName, methodBody);

				if(staticF)
					pos().error("Cannot preface non-class function with keyword static");
			} else {
				Statement* funcName;
				//TODO change scope
				//myMod = nullptr;//make combined scope
				if(outer==nullptr){
					funcName = new E_VAR(Resolvable(data.mod, methodName[0].first, pos()), false);
					for(unsigned int i = 1; i+2<methodName.size(); i++)
						funcName = new E_LOOKUP(methodName[i].second, funcName, methodName[i].first, false);
				} else{
					funcName = outer;//new E_LOOKUP(methodName[0].second, outer, methodName[0].first);
					for(unsigned int i = 0; i+2<methodName.size(); ++i){
						funcName = new E_LOOKUP(methodName[i].second, funcName, methodName[i].first, false);
					}
				}
				if((outer!=nullptr)?(methodName.back().first == outer->name):(methodName.back().first==methodName[methodName.size()-2].first)){
					if(staticF)
						pos().error("Cannot precede constructor with static keyword");
					func = new ConstructorFunction(pos(), data.mod,funcName);
					//arguments, Resolvable(module,"this",pos()), funcName, methodBody);
				}
				else{
					if(temp=="gen"){
						pos().compilerError("Class-based generators not implemented yet");
						exit(1);
						//func = new E_GEN(tmp, arguments, funcName, returnName, methodBody,methodName.back().first, Resolvable(module,"this",pos()));
					}
					else
						func = new ClassFunction(pos(), data.mod, methodName.back().first, staticF, funcName);
					//, arguments, staticF, Resolvable(module,"this",pos()), methodName.back().first, funcName, returnName, methodBody);
				}
			}
			func->returnV = returnNameTemp;
			if(!f->done){
				if(f->peek()=='('){
					f->read();
					parseArguments(func->declaration,ParseData(EOF, & func->module,true,PARSE_LOCAL),')');
				}
				else f->error("Need '(' in function declaration, found "+String(1,f->read()),true);
			}
			trim(data);
			if(!f->done && f->peek()==':'){
				f->read();
				trim(data);
			}
			bool paren;
			func->methodBody = getNextBlock(ParseData(data.endWith, & func->module, true,PARSE_LOCAL),&paren);
			trim(data);
			//bool semi  = false;
			//if(!f->done && f->peek()==';'){ semi = true; }
			//trim(data);
			//if(data.operatorCheck && !semi) return operatorCheck(data, func);
			return func;
		}
		else if (temp == "extern"){
			auto retV = getNextType(ParseData(EOF, data.mod,true,data.loc),true);
			if(!retV) pos().error("Cannot use auto return type for external function");
			trim(data);
			String methodName = getNextName(EOF);
			data.mod->addFunction(pos(), methodName);
			auto EXTERN = new ExternFunction(pos(), data.mod,methodName);
			EXTERN->returnV = retV;
			//todo check if necessary
			trim(data);
			if(f->peek()!='('){
				f->error("'(' required after extern not "+String(1,f->peek()),true);
			}
			f->read();
			while(true){
				trim(data);
				if(f->peek()==')') break;
				Declaration* d= getNextDeclaration(ParseData(EOF, & EXTERN->module, true,PARSE_LOCAL),false,false,true,true);
				EXTERN->declaration.push_back(d);
				trim(data);
				if(f->peek()==',') f->read();
			}
			if(f->read()!=')') f->error("Need ending ')' for extern", true);
			return EXTERN;
		}
		else{
			f->error("Invalid function start -- used "+temp);
			return nullptr;
		}

	}
	OClass* getClass(ParseData data, bool read=false,OClass* outer=nullptr,bool stat/*ic*/=true){
		if(!read && f->getNextName(data.endWith)!="class") f->error("Could not find 'class' for class declaration");
		String name = getNextName(EOF);//TODO -- allow generic class definition
		LayoutType primitive;
		if(name=="primitive" || name=="primitive_pointer"){
			primitive = (name=="primitive")?PRIMITIVE_LAYOUT:PRIMITIVEPOINTER_LAYOUT;
			name = getNextName(EOF);//TODO -- allow generic class definition (here as well)
		} else primitive = POINTER_LAYOUT;
		if(f->trim(EOF)) f->error("No class defined!");
		Statement* superClass=nullptr;
		if(f->peek()==':'){
			f->read();
			superClass = getNextType(data.getEndWith(EOF),true);
			if(superClass==nullptr) pos().error("Cannot have auto as superclass");
			f->trim(EOF);
		}
		if(f->read()!='{') f->error("Need opening brace for class definition");
		f->trim(EOF);
		bool isFinal = false /*todo parse*/;
		const AbstractClass* superC;
		if(superClass==nullptr){
			if(primitive==POINTER_LAYOUT) superC = &objectClass;
			else superC = nullptr;
		} else superC = superClass->getMyClass(getRData(), pos());
		//todo register in outer class instead of whereever this place is...
		if(superC){
			assert(superC);
			if(superC->layout!=primitive)
				f->error("Cannot have a class with a superclass of a different layout type "+superC->getName()+" "+name+" "+str<LayoutType>(primitive),true);
			if(superC->isFinal)
				f->error("Cannot create subclass to final class");
		}
		UserClass* proto = new UserClass(data.mod, name, superC,primitive, isFinal);
		data.mod->addClass(pos(), proto);
		OModule* classMod = new OModule(data.mod);
		OClass* selfClass = new OClass(pos(), proto,(!stat)?outer:(nullptr));
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
			ParseData nd(EOF, classMod, true, (stat)?data.loc:PARSE_LOCAL);
			if(temp=="class"){
				if(stat!=true) pos().error("Optricks does not currently support non-static classes");
				auto uClass = getClass(nd, true, selfClass, stat);
				selfClass->under.push_back(uClass);
			} else if(temp=="def" || temp=="gen" || temp=="inl"){
				Statement* func = getFunction(nd, temp,proto,stat);
				selfClass->under.push_back(func);
			} else if(temp=="extern"){
				pos().error("Optricks does not allow the use of external functions in the definition classes!");
			} else{
				f->undoMarker(mark);
				Declaration* dec = getNextDeclaration(nd,stat,true,false);
				dec->variable.pointer.module = &(proto->staticVariables);
				proto->staticVariables.addVariable(pos(), dec->variable.pointer.name,new DeclarationData(dec));

				selfClass->data.push_back(std::pair<bool,Declaration*>(stat,dec));
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
	Statement* getEnum(ParseData data, bool read=false,OClass* outer=nullptr,bool stat/*ic*/=true){
		if(!read && f->getNextName(data.endWith)!="enum") f->error("Could not find 'enum' for enum declaration");
		String name = getNextName(EOF);//TODO -- allow generic class definition
		if(f->trim(EOF)) f->error("No class defined!");
		if(f->read()!='{') f->error("Need opening brace for enum definition");
		f->trim(EOF);
		//TODO should enums have local functions / static vars / inner classes / etc
		std::vector<String> s;
		while(true){
			auto tmp = f->peek();
			if(tmp=='}' || tmp==EOF) break;
			else if(tmp==';'){
				f->read();
				f->trim(EOF);
				continue;
			} else if(tmp==','){
				f->read();
				f->trim(EOF);
				continue;
			} else {
				if(!isStartName(tmp)){
					f->error("Need name for enum internal -- fonud "+String(1,tmp));
					f->read();
					f->trim(EOF);
					continue;
				}
				else{
					s.push_back(f->getNextName(EOF));
					f->trim(EOF);
				}
			}
		}
		if(f->read()!='}') f->error("Need closing brace for class definition");
		EnumClass* proto = new EnumClass(data.mod, name, s);
		data.mod->addClass(pos(), proto);
		while(f->peek()==';'){
			f->read();
			f->trim(EOF);
		}
		return proto;
	}
	bool isValidType(ParseData data){
		f->trim(data.endWith);

		auto s = f->getNextName(data.endWith);
		if(s.length()==0) return false;
		f->trim(data.endWith);

		auto fp = f->peek();
		if(fp=='.'){
			f->read();
			return isValidType(data);
		} else if(fp=='{'){
			f->read();
			f->trim(EOF);
			while(true){
				auto z = f->peek();
				if(z==EOF){
					pos().error("Unclosed '{' for template", false);
					return false;
				}
				else if(z=='}'){
					f->read();
					break;
				} else if(z==','){
					f->read();
					f->trim(EOF);
				}
				if(!isValidType(data.getEndWith(EOF))) return false;
				f->trim(EOF);
			}
		}
		while((fp=='&' || fp=='%' || fp=='[')){
			if(f->read()=='['){
				f->trim(EOF);
				//TODO allow int[4]
				if(f->peek()!=']'){
					return false;
				}
				f->read();
			}
			f->trim(data.endWith);
			fp = f->peek();
		}
		return true;

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
						else stack.push_back(nullptr);
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
			if(f->peek()=='=' && (f->read()||true) && f->peek()!='='){
				Statement* op2 = getNextStatement(data);
				if(nex->getToken()==T_UOP){
					//append operator
					E_UOP* u = (E_UOP*)nex;
					nex = E_BINOP::createBinop(pos(), u->value, op2, "[]=");
					delete u;
				} else {
					//TODO ternary operator #[#] = #
					pos().warning("TODO ternary operator #[#] = #");
					nex = new E_SET(pos(), nex,op2);
				}
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
			//TODO do better job of this actually parsing arguments
			Statement* e = getNextStatement(ParseData(data.endWith, data.mod, false,PARSE_EXPR));
			Statement* ret;
			if(e->getToken()==T_PARENS){
				auto IN = ((E_PARENS*)e)->inner;
				delete e;
				ret = new E_FUNC_CALL(pos(), exp, std::vector<const Evaluatable*>(1,
						(IN->getToken()==T_VOID)?nullptr:IN));
			}
			else{
				auto V = ((E_TUPLE*)e)->values;
				std::vector<const Evaluatable*> E;
				for(auto& a: V)
					if(a->getToken()==T_VOID) E.push_back(nullptr);
					else E.push_back(a);
				delete e;
				ret = new E_FUNC_CALL(pos(), exp, E);
			}

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
			pos().compilerError(" '{' operatorCheck not implemented yet");
			exit(1);
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
			return new E_UOP(pos(), "!", exp, UOP_POST);
		}
		//TODO implement generics
		/*
			else if (tmp == "<"){
				//equality and check
			}
		 */ //TODO implement custom operators (a and b,  r if g else b )
		else if (tmp=="."){
			String name = getNextName(data.endWith);
			fixed = getLookup(data, exp, name);
		}
		else if(tmp=="="){
			Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
			fixed = new E_SET(pos(), exp, post);
		}
		else if(tmp=="++" || tmp=="--"){
			fixed = new E_UOP(pos(), tmp, exp, UOP_POST);
		}
		else if(tmp.size()>=2 && tmp[tmp.size()-1]=='=' && !(tmp[tmp.size()-2]=='=' || tmp[tmp.size()-2]=='!' || (tmp.size()==2 && (tmp[0]=='<' || tmp[0]=='>')))){
			tmp = tmp.substr(0,tmp.size()-1);
			Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
			fixed = new E_SET(pos(), exp, E_BINOP::createBinop(pos(), exp, post, tmp));
		}
		else{
			Statement* post = getNextStatement(ParseData(data.endWith, data.mod, true, PARSE_EXPR));
			if(post->getToken()==T_VOID)
				fixed = new E_UOP(pos(), tmp, exp, UOP_POST);
			else fixed = E_BINOP::createBinop(pos(), exp, post,tmp);
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
	if(f->done || f->trim(EOF)) return VOID_STATEMENT;
	int nex = f->peek();
	if(f->done || nex==EOF || nex==','||nex==')' ||nex==']'/*|| nex==endWith*/) return VOID_STATEMENT;
	if(nex==';') return VOID_STATEMENT;
	bool semi;
	if (isStartName(nex)) {
		auto undoRead = f->getMarker();
		String temp = f->getNextName(data.endWith);
		if(temp=="class") return getClass(data, true);
		else if(temp=="enum") return getEnum(data, true);
		else if(temp=="if") return getIfStatement(data,true);
		else if(temp=="for") return getForLoop(data,true);
		else if(temp=="while") return getWhileLoop(data,true);
		else if(temp=="do") return getDoLoop(data,true);
		else if(temp=="lambda") return getLambdaFunction(data,true);
		else if(temp=="gen" || temp=="def" || temp=="inl" || temp=="extern") return getFunction(data,temp);
		else if(temp=="import"){
			trim(data);
			String tI = f->readString(data.endWith);
			if(tI.length()==0) f->error("Could not import file of no length");
			return new ImportStatement(pos(),tI);
		}
		else if (temp=="true" || temp=="false"){
			Statement* te = (temp=="true")? ConstantData::getTrue() : ConstantData::getFalse();
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
			Statement* t = VOID_STATEMENT;
			//TODO prevent returns from use in iterator
			if(f->peek()!='}' && f->peek()!=',' && f->peek()!=':' && f->peek()!=')') t = getNextStatement(data.getLoc(PARSE_EXPR));
			return new E_RETURN(pos(), t, "", (temp=="return")?RETURN:YIELD);
		}
		else if(temp=="break" || temp=="continue"){
			//			if(!data.allowsDec()) f->error("Cannot have return here");
			trim(data.endWith);
			String name = (isStartName(f->peek()))?(getNextName(EOF)):"";
			return new E_RETURN(pos(), nullptr, name, (temp=="break")?BREAK:CONTINUE );
		}
		else{
			f->undoMarker(undoRead);
			f->trim(data.endWith);
			//auto A = (char)f->peek();
			//cerr << "A " << (int)A << " " << (char)A << endl << flush;
			//cerr << "LAST " << (int)(f->last()) << " " << (char)(f->last()) << endl << flush;
			//cerr << "STATS: " << data.allowsDec() << " and " << f->interactive << " " << (f->last()=='\n') << endl << flush;
			if(data.allowsDec() && isValidType(data)){
				f->trim(data.endWith);
				//auto B = (char)f->peek();
				//cerr << "B " << (int)B << " " << (char)B << endl << flush;
				//auto tmp = f->getMarker();
				//String s = String(tmp-undoRead, ' ');
				//f->undoMarker(undoRead);
				//while(f->getMarker()!=tmp) s[f->getMarker()-undoRead]=f->read();
				//cerr << "READ: " << s << endl << flush;
				//auto C = (char)f->peek();
				//cerr << "C " << (int)C << " " << (char)C << endl << flush;
				if(!(f->interactive && f->last()=='\n') && isStartName(f->peek()) ){
					f->undoMarker(undoRead);
					return getNextDeclaration(data);
				}
			}
			f->undoMarker(undoRead);
			//auto D = (char)f->peek();
			//cerr << "D " << (int)D << " " << (char)D << endl << flush;
			E_VAR* nextVar = getNextVariable(data,true,true);
			if(!nextVar) pos().error("Cannot use 'auto' as variable");
			semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			trim(data);
			if(data.operatorCheck && !semi)
				return operatorCheck(data, nextVar);
			else return nextVar;
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
			Literal* num = f->readNumber(data.endWith);
			trim(data);
			semi  = false;
			if(!f->done && f->peek()==';'){ semi = true; }
			trim(data);
			if(data.operatorCheck && !semi) return operatorCheck(data, num);
			else return num;
		}
		case '\'':
		case '"':{
			String s = f->readString(data.endWith);
			Statement* str = (s.length()==1)?((Statement*) new ConstantData(CharClass::getValue(s[0]), &charClass))
					: ((Statement* )new StringLiteral(s));
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
					if((te = f->read())!=close) f->error("Cannot end inline paren with "+
							String(1,te)+" instead of "+String(1,close)
					);
					temp = new E_PARENS(temp);
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
			Statement* arr = (open=='(')?((Statement*)(new E_TUPLE(values))):(
					(open=='[')?((Statement*)(new E_ARR(pos(), values))):
							((Statement*)(new E_ARR(pos(), values))) //todo change to E_SET
			);



			auto start = f->getMarker();
			trim(data);
			if(data.allowsDec() && (!f->interactive || f->last()!='\n') && start!=f->getMarker() && isStartName(f->peek()) ){
				//auto tmp = f->getMarker();
				f->undoMarker(undoRead);
				//TODO FIX HERE TEMPLATES / lazy / ref
				//String s = String(tmp-undoRead, ' ');
				//while(f->getMarker()!=tmp) s[f->getMarker()-tmp]=f->read();
				//cerr << s << endl << flush;
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
				Statement* toReturn = new E_UOP(pos(), temp,getNextStatement(data.getLoc(PARSE_EXPR)), UOP_PRE);
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
	return VOID_STATEMENT;
}
#endif /* LEXER_HPP_ */
