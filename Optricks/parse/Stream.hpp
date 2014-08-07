/*
 * Stream.hpp
 *
 *  Created on: Jan 21, 2014
 *      Author: Billy
 */

#ifndef STREAM_HPP_
#define STREAM_HPP_

#include "../language/data/literal/IntLiteral.hpp"
#include "../language/data/literal/FloatLiteral.hpp"
#include "../language/data/literal/ImaginaryLiteral.hpp"
#include "../language/data/literal/StringLiteral.hpp"
#include "../language/data/literal/SliceLiteral.hpp"

#include "../ast/E_BINOP.hpp"
#include "../ast/E_UOP.hpp"

#define START "optricks> "
#define CONT  "--------> "

template<typename C> bool in(const std::vector<C> a, C b){
	for(const auto& e: a)
		if(e==b) return true;
	return false;
}

const bool isWhitespace(const char a){
	return a==' ' || a=='\n' || a=='\t' || a=='\r';
}

bool isStartName(int i){
	return isalpha((char)i) || i=='_' || i=='$';
}

bool isStartType(int i){
	return isalpha((char)i) || i=='_' || i=='$' || i=='{' || i=='(' || i=='[';
}

const bool isOperator(const char a){
	return  a=='!' || a=='%' || a=='&' ||
			a=='^' || a=='*' || a=='|' ||/* a=='(' || a==')' ||*/
			a=='-' || a=='+' || a=='=' || /*a=='[' || a==']' ||*/ a=='<' ||
			a=='>' || a=='.' || a=='/' || a=='\\' || a=='|' ||
			/*a=='{' || a=='}' ||*/ a=='?';
}

const std::vector<String> RESERVED_KEYWORDS = {
		"if","else","elif","for","while","do",
		"lambda","def","gen","extern",
		"true","false",
		"return","break","continue"
};

const std::vector<String> PRE_OPERATORS = {"++", "--", "+", "-", "&", "*", "!", "~"};
const std::vector<String> BINARY_OPERATORS = {"and", "or", "xor","xnor",
		".",":","::","->",".*",":*","::*","->*","=>*",
		"*^" ,"<>",
		"++","--","%%",
		"**","^","%","*","/","+","-",
		"<<",">>","<<<",">>>",
		"<=","<",">",">=","==","===","!=","!==",
		"&","^^", "|","&&","||",
		"\\.",
		"+=","%=","-=" ,"*=", "\\.=",
		"/=","//=","**=","^=","|=","||=","&=","&&=","^^=",
		"=", ":=", "::=", "<<=", ">>=", "<<<", ">>>", "[", "{","("
};

class Stream{
private:
	FILE* f;
	String fileName;
public:
	const bool interactive;
private:
	std::vector<int> cache;
	std::vector<std::vector<int> > readChars;
	unsigned int curCount;
	void * operator new   (size_t)=delete;
	void * operator new[] (size_t)=delete;
	void writeNonLine(int c){
		//done = false;
		assert(c!='\n');
		assert(c!=EOF);
		assert(readChars.size()>0);
		assert(readChars.back().size()>0);
		assert(readChars.back().back()==c);
		cache.push_back(c);
		readChars.back().pop_back();
		curCount--;
	}
	void writePLine(int c){
		done = false;
		assert(c!=EOF);
		assert(readChars.size()>0);
		assert(readChars.back().size()>0);
		assert(readChars.back().back()==c);
		if(c=='\n'){
			assert(readChars.back().empty());
			cache.push_back('\n');
			readChars.pop_back();
		} else {
			cache.push_back(c);
			readChars.back().pop_back();
		}
		curCount--;
	}
	void write(){
		done = false;
		if(readChars[readChars.size()-1].empty()){
			assert(readChars.size()!=0 && "Wrote more than available chars...?");
			readChars.pop_back();
			cache.push_back('\n');
		}
		else{
			cache.push_back(readChars.back().back());
			readChars.back().pop_back();
		}
		curCount--;
	}
public:
	bool enableOut;
	friend Lexer;
	Stream(String file, bool i): interactive(i){
		enableOut = false;
		done = false;
		curCount = 0;
		allowUndo = false;
		start = true;

		FILE* fileV;
		if(file=="-" || file=="" || file=="<stdin>"){
			file="<stdin>";
			fileV = stdin;
		} else{
			fileV = fopen(file.c_str(), "r");
			if(fileV==NULL){
				cerr << "Error opening file " << file << endl << flush;
				char cwd[1024];
				if(getcwd(cwd,sizeof(cwd))==NULL) pos().error("Could not get Current Working Directory (2)");
				cerr << cwd << endl << flush;
				exit(1);
			}
		}
		f = fileV;
		fileName = file;
		readChars.push_back(std::vector<int>());
	}

	char last(){
		if(readChars.size()==0) return ' ';
		else if(readChars.back().size()==0) return '\n';
		else return readChars.back().back();
	}
	void force(String c){
		for(unsigned int i = c.size()-1; ; i--){
			cache.push_back(c[i]);
			if(i==0) break;
		}
		std::cout << c << flush;
	}
	/*
	void write(char c){
		done = false;
		if(readChars[readChars.size()-1].empty()){
			if(readChars.size()==0){
				error("Wrote more than available chars...?",true);
			}
			if(c!='\n'){
				error("Cannot re-insert non \\n",true);
			}
			readChars.pop_back();
			cache.push_back('\n');
		}
		else{
			char t = readChars.back().back();
			if(t!=c){
				error("Cannot re-insert incorrect char",true);
			}
			cache.push_back(t);
			readChars.back().pop_back();
		}
		curCount--;
	}
	void write(String c){
		for(unsigned int i = 0; i<c.size(); ++i){
			write(c[i]);
		}
	}*/
	unsigned int getMarker(){
		return curCount;
	}
	void undoMarker(unsigned int a){
		while(curCount>a) write();
	}
	int read(){
		if(done) error("Already done reading file");
		int c;
		if(cache.size()==0){
			if(readChar(f,&c)){
				error("Cannot read from file",true);
			}
			if(c=='\n' && interactive && enableOut){
				std::cout << CONT << flush;
			}
		}
		else{
			c = cache.back();
			cache.pop_back();
		}
		if(c=='\r'){
			do{
				if(readChar(f,&c)){
					error("Cannot read from file",true);
				}
			}while(c=='\r');
			if(c=='\n' && interactive && enableOut){
				std::cout << CONT << flush;
			}
		}
		if(c=='\n'){
			readChars.push_back(std::vector<int>());
		} else readChars.back().push_back(c);
		curCount++;
		return c;
	}
	//to follow a peek if meant to be read
	inline void move1(int c){
		assert(cache.size()>0);
		assert(c==cache.back());
		if(c=='\n'){
			readChars.push_back(std::vector<int>());
		} else readChars.back().push_back(c);
		curCount++;

		cache.pop_back();
	}
	int peek(){
		if(cache.size()>0) return cache.back();
		int c;
		if(readChar(f,&c)){
			error("Cannot peek from file",true);
		}
		cache.push_back(c);
		return c;
	}
	bool start;
	bool allowUndo;
	bool done;
	PositionID pos() const {
		assert(this);
		return PositionID(readChars.size(), readChars.back().size(),fileName);
	}
	virtual ~Stream(){
		fclose(f);
	}
	//checks if is single equals (not ==) and reads the single equals if so
	//otherwise leaves unchanged
	bool isSingleEquals(){
		if(peek()!='=') return false;
		move1('=');
		if(peek()=='='){
			writeNonLine('=');
			return false;
		}
		else
			return true;
	}
	unsigned int peekOctDigit(){
		char c = peek();
		if(c>='0' && c<='7') return c-'0';
		else{
			error("Did not recognize hex character");
			exit(0);
		}
	}
	unsigned int readOctDigit(){
		char c = read();
		if(c>='0' && c<='7') return c-'0';
		else{
			error("Did not recognize hex character");
			exit(0);
		}
	}
	unsigned int peekHexDigit(){
		char c = peek();
		if(c>='0' && c<='9') return c-'0';
		else if(c>='a' && c<='f') return c-'a';
		else if(c>='A' && c<='F') return c-'A';
		else{
			error("Did not recognize hex character");
			exit(0);
		}
	}
	unsigned int readHexDigit(){
		char c = read();
		if(c>='0' && c<='9') return c-'0';
		else if(c>='a' && c<='f') return c-'a';
		else if(c>='A' && c<='F') return c-'A';
		else{
			error("Did not recognize hex character");
			exit(0);
		}
	}

	/**
	 * Precondition: already read the quote
	 */
	String readString(char endWith, bool isDouble){
		String filling = "";
		int escape = 0;
		do{
			auto front = read();
			if(front==EOF){
				pos().error("Unclosed string literal");
				exit(1);
			}
			if(escape == 0){
				switch(front){
				case '"':
					if(isDouble){
						goto doneLoop;
					}
					else filling+=front;
					break;
				case '\'':
					if(!isDouble){
						goto doneLoop;
					}
					else filling+=front;
					break;
				case '\\':
					escape = 1;
					break;
				default:
					filling+=front;
					break;
				}
			}
			else if(escape==1){
				unsigned int n;
				switch(front){
				case '\\':
					filling+='\\';
					escape = 0;
					break;
				case '\r':
					if(peek()=='\n') move1('\n');
					escape = 0;
					break;
				case '\n':
					escape =0;
					break;
				case '\'':
					escape = 0;
					filling+='\'';
					break;
				case '"':
					escape = 0;
					filling+='"';
					break;
				case 'a':
					escape = 0;
					filling+='\a';
					break;
				case 'b':
					escape = 0;
					filling+='\b';
					break;
				case 'f':
					escape = 0;
					filling+='\f';
					break;
				case 'n':
					escape = 0;
					filling+='\n';
					break;
				case 'r':
					escape = 0;
					filling+='\r';
					break;
				case 't':
					escape = 0;
					filling+='\t';
					break;
				case 'u':
					n = 0;
					for(int i = 0; i<4; i++){
						n<<=4;
						n|=readHexDigit();
					}
					escape = 0;
					filling+=n;
					break;
				case 'U':
					n = 0;
					for(int i = 0; i<8; i++){
						n<<=4;
						n|=readHexDigit();
					}
					escape = 0;
					filling+=n;
					break;
				case 'v':
					escape = 0;
					filling+='\v';
					break;
				case '0':
					escape = 0;
					filling+='\0';
					break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					//WHAT IS THIS!?!?
					error("What is this strange string parse");
					n = 0;
					for(int i = 0; i<3; i++){
						n<<=3;
						n|=readOctDigit();
					}
					escape = 0;
					filling+=n;
					break;
				case 'h':
				case 'x':
					n = 0;
					for(int i = 0; i<2; i++){
						n<<=4;
						n|=readHexDigit();
					}
					filling+=n;
					escape = 0;
					break;
				}
			}
		}while(true);

		doneLoop:
		trim(endWith);
		if(done) return filling;
		auto tmp = peek();
		if(tmp=='"' || tmp=='\''){
			move1(tmp);
			return filling+readString(endWith,tmp=='"');
		} else{
			return filling;
		}
	}

	String readWhile(const bool (*fun)(char)){
		String st;
		do{
			auto load = peek();
			if(!fun(load)){
				break;
			} else move1(load);
			st+=load;
		}while(true);
		return st;
	}
	/*String peekWhile(const bool (*fun)(char)){
		String st = readWhile(fun);
		for(unsigned int i = 0; i<st.size(); ++i) write();
		return st;
	}
	String readUntil(char c){
		String st;
		while(peek()!=c && peek()!=EOF) st+=read();
		return st;
	}
	String peekUntil(char c){
		String st = readUntil(c);
		for(unsigned int i = 0; i<st.size(); ++i) write();
		return st;
	}
	String read(unsigned int len){
		String c;
		for(unsigned int i = 0; i<len;i++) c+=read();
		return c;
	}
	String peek(unsigned int len){
		String c = read(len);
		for(unsigned int i = 0; i<len;i++) write();
		return c;
	}*/

	void error(String a = "Compile error", /*String b="", String c="", String d="",*/ bool end=false){
		pos().error(a, end);
	}

	//bool startsWith(String s){
	//	return s==peek(s.length());
	//}

	bool trimNonLine(char endWith=EOF){
		if(done){
			return true;
		}
		do{
			auto c = peek();
			if(c==EOF){
				done = true;
				return true;
			} else if(c==endWith){
				return false;
			} else if(c==' ' || c=='\t' || c=='\r') {
				move1(c);
			} else {
				break;
			}
			/*	else if(endAtLines && c=='\n'){
				done = true;
				return true;
			}*/
		}while(true);
		return false;
	}

	bool trimEndOfLine(){
		if(done){
			return true;
		}
		do{
			auto c = peek();
			if(c==EOF){
				done = true;
				return true;
			} else{
				move1(c);
				if(c=='\n') break;
			}
		}while(true);
		if(peek()=='\r') move1('\r');
		//done |= endAtLines;
		return done;//endAtLines;
	}

	/**
	 * Returns whether the file has ended
	 */
	bool trim(char endWith/*=EOF*/){
		if(done) return true;
		auto tmp = curCount;
		do{
			tmp = curCount;
			if(trimNonLine(endWith)){
				return true;
			}
			auto c = peek();
			if(c==EOF /*|| (endAtLines && c=='\n')*/){
				done = true;
				return true;
			} else if(c==endWith){
				return false;
			} else if(c=='\n'){
				move1('\n');
				continue;
			} else if(c=='#'){
				if(trimEndOfLine()) return true;
			} else if(c=='/'){
				move1('/');
				auto n = peek();
				if(n=='/'){
					if(trimEndOfLine()) return true;
				} else if(n=='*'){
					char prev = ' ';
					char cur = ' ';
					do{
						prev = cur;
						cur = peek();
						if(cur==EOF){
							error("Unclosed /* comment");
							exit(0);
							return true;
						} else move1(cur);
					}while(prev!='*' || cur!='/');
				} else{
					writeNonLine('/');
				}
			}
		}while(tmp!=curCount);
		return false;
	}
	std::vector<String> getCommaSeparated(char endWith){
		std::vector<String> vals;
		do{
			if(trim(endWith)) return vals;
			String temp = getNextName(endWith);
			if(temp.size()==0) return vals;
			vals.push_back(temp);
			if(trim(endWith)) return vals;
			if(done) return vals;
			if(peek()!=',') return vals;
			else move1(',');
		}
		while(true);
		return vals;
	}
	String getNextName(int endWith){
		if(done) return "";
		char nex = peek();
		if(nex==endWith){ done=true; return ""; }
		if (isalpha((char)nex) || nex=='$' || nex=='_') {
			String temp = "";
			char tchar;
			do{
				tchar = peek();
				if(endWith==tchar){
					done = true;
					return temp;
				}
				if(isalnum(tchar) || tchar=='$' || tchar=='_'){
					temp+=tchar;
					move1(tchar);
				}
				else break;
			}while(true);
			return temp;
		}
		return "";
	}
	/**
	 * Reads a number (not preceded by leading + or -)
	 */
	Literal* readNumber(char endWith, bool negative){
		assert(endWith < '0' || endWith > '9');
		assert(endWith != 'b');
		assert(endWith != 'B');
		assert(endWith != 'x');
		assert(endWith != 'X');
		assert(endWith != '-');
		assert(endWith != '+');
		if(done) return &ZERO_LITERAL;
		String hi=negative?"-":"";
		char tchar=peek();
		int base;
		if(tchar=='0'){
			move1('0');
			tchar = peek();
			if(tchar=='x' || tchar=='X'){
				move1(tchar);
				base=16;
				tchar = peek();
			}
			else if(tchar=='b' || tchar=='B'){
				move1(tchar);
				base = 2;
				tchar = peek();
			}
			else if(tchar>='0' && tchar<='7'){
				move1(tchar);
				base = 8;
				hi+=tchar;
				tchar = peek();
			} else base = 10;
		} else base = 10;

		bool decimal = false;
		bool exponent = false;
		do{
			if(done) break;
			if(tchar==endWith){
				done=true;
				break;
			} else if(tchar=='.'){
				move1('.');
				hi+=tchar;
				decimal = true;
			} else if(base<15 && !exponent && (tchar=='e' || tchar=='E')){
				move1(tchar);
				hi+=tchar;
				exponent = true;
				tchar = peek();
				if(tchar=='-' || tchar=='+') hi+=tchar;
				else continue;
			}
			else if(isdigit(tchar) || (tchar>='a' && tchar<='f')  || (tchar>='A' && tchar<='F')){
				move1(tchar);
				hi+=tchar;
			}
			else{
				break;
			}
			tchar = peek();
		}while(true);

		Literal* tmp = (decimal)?((Literal*)(new FloatLiteral(hi.c_str(),base))):(
				(Literal*)(new IntLiteral(hi.c_str(),base)));
		if(done) return tmp;
		auto pek = peek();
		if(pek=='i' || pek=='j' || pek=='I'){
			move1(tchar);
			return new ImaginaryLiteral(nullptr,tmp);
		}
		else return tmp;
	}
	String getNextOperator(char endWith){
		trim(endWith);
		String temp;
		int valid=0;
		while(true){
			auto tmp = peek();
			if(!isOperator(tmp)){
				unsigned int i=temp.length();
				if(i==valid) return temp;
				while(true){
					writeNonLine(temp[i-1]);
					if(i==valid+1) break;
					else i--;
				}
				temp.resize(valid);
				return temp;
			}
			String t2=temp+String(1,tmp);
			if(in<String>(BINARY_OPERATORS, t2)) valid=t2.length();
			move1(tmp);
			temp = t2;
		}
	}

};


Statement* getIndex(Stream* f, Statement* toIndex, std::vector<Statement*>& stack){
	if(stack.size()==0){
		return new E_UOP(f->pos(), "[]",toIndex, UOP_POST);
		//TODO HANDLE APPEND OPERATORS
		//f->error("Append operators not implemented yet",true);
		//return toIndex;
	}
	if(stack.size()==1 && stack[0]!=NULL){
		Statement* temp = stack[0];
		stack.clear();
		return E_BINOP::createBinop(f->pos(), toIndex,temp,"[]");
	}
	else{
		//TODO allow for a[::,2] or a[:,:]
		/*Statement* start = NULL, *end=NULL,*step=NULL;
		unsigned int pos = 0, spos = 0;
		while(stack.size()>spos){
			if(stack[spos]==NULL){
				pos++;
				spos++;
			}
			else{
				switch(pos){
				case 0:
					start = stack[spos];
					break;
				case 1:
					end = stack[spos];
					break;
				case 2:
					step = stack[spos];
					break;
				default:
					f->error("stack not empty when parsing slice");
				}
				spos++;
			}
		}
		stack.clear();*/
		f->pos().compilerError("Slicing not implemented yet!");
		exit(1);
		//auto e = new SliceLiteral(f->pos(), start,end,step);
		//return new E_BINOP(f->pos(), toIndex,e,"[]");
	}
}

#endif /* STREAM_HPP_ */
