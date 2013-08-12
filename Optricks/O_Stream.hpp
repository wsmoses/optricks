#ifndef O_STREAM_HPP_
#define O_STREAM_HPP_

const bool isWhitespace(const char a){
	return a==' ' || a=='\n' || a=='\t' || a=='\r';
}

const bool isOperator(const char a){
	return  a=='!' || a=='%' || a=='&' ||
			a=='^' || a=='*' || a=='|' ||/* a=='(' || a==')' ||*/
			a=='-' || a=='+' || a=='=' || /*a=='[' || a==']' ||*/ a=='<' ||
			a=='>' || a=='.' || a=='/' || a=='\\' || a=='|' ||
			/*a=='{' || a=='}' ||*/ a=='?';
}

const String BINARY_OPERATORS[]{"and", "or", "xor","xnor",
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

#include "O_IO.hpp"
#include "expressions/E_INDEXER.hpp"
#include "expressions/E_LOOKUP.hpp"
#include "expressions/E_BINOP.hpp"
#include "expressions/E_PARENS.hpp"
#include "expressions/E_FUNC_CALL.hpp"
#include "constructs/Expression.hpp"

class Stream{
private:
	FILE* f;
	std::vector<char> cache;
	std::vector<std::vector<char> > readChars;
	unsigned int curCount;
	void write(){
			done = false;
			if(readChars[readChars.size()-1].empty()){
				if(readChars.size()==0){
					error("Wrote more than available chars...?",true);
				}
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
	char last(){
		if(readChars.size()==0) return ' ';
		else if(readChars.back().size()==0) return '\n';
		else return readChars.back().back();
	}
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
	}
	unsigned int getMarker(){
		return curCount;
	}
	void undoMarker(unsigned int a){
		while(curCount>a) write();
	}
	char read(){
		if(done) error("Already done reading file");
		char c;
		if(cache.size()==0){
			if(readChar(f,&c)){
				error("Error reading from file",true);
			}
		}
		else{
			c = cache.back();
			cache.pop_back();
		}
		if(c=='\r') return read();
		if(c=='\n'){
			readChars.push_back(std::vector<char>());
		} else readChars.back().push_back(c);
		curCount++;
		return c;
	}
	char peek(){
		char c = read();
		write();
		return c;
	}
	bool start;
	bool allowUndo;
	bool done;
	bool endAtLines;
	Stream(FILE* a, bool lines=false){
		done = false;
		curCount = 0;
		allowUndo = false;
		endAtLines = lines;
		start = true;
		f = a;
		readChars.push_back(std::vector<char>());
	}
	String readRecursiveI(){
		String filling = "";
		std::vector<char> st;
		do{
			char front = read();
			if(st.size()==0){
				//TODO reimplement front
				//if(front==EOF){ write(front); return filling; }
				switch(front){
				case '"': st.push_back('"'); break;
				case '\'': st.push_back('\''); break;
				//	case '#': st.push_back('#'); break;
				case '(': st.push_back('('); break;
				case '[': st.push_back('['); break;
				case '{': st.push_back('{'); break;
				case ')': error("Found ')' where not expected"); exit(0);
				case ']': error("Found ']' where not expected"); exit(0);
				case '}': error("Found '}' where not expected"); exit(0);
				}
			}
			else{
				switch(st.back()){
				case '\\':
					st.pop_back();
					break;
				case '"':
					if(front=='\\') st.push_back('\\');
					else if(front=='"') st.pop_back();
					break;
				case '\'':
					if(front=='\\') st.push_back('\\');
					else if(front=='\'') st.pop_back();
					break;
					//		case '#':
					//			if(front=='#') st.pop_back();
					//			break;
				case '(':
					switch(front){
					case '"': st.push_back('"'); break;
					case '\'': st.push_back('\''); break;
					case '#': st.push_back('#'); break;
					case '(': st.push_back('('); break;
					case '[': st.push_back('['); break;
					case '{': st.push_back('{'); break;
					case ')': st.pop_back(); break;
					case ']': error("Found ']' where not expected"); exit(0);
					case '}': error("Found '}' where not expected"); exit(0);
					}
					break;
					case '[':
						switch(front){
						case '"': st.push_back('"'); break;
						case '\'': st.push_back('\''); break;
						case '#': st.push_back('#'); break;
						case '(': st.push_back('('); break;
						case '[': st.push_back('['); break;
						case '{': st.push_back('{'); break;
						case ')': error("Found ')' where not expected"); exit(0);
						case ']': st.pop_back(); break;
						case '}': error("Found '}' where not expected"); exit(0);
						}
						break;
						case '{':
							switch(front){
							case '"': st.push_back('"'); break;
							case '\'': st.push_back('\''); break;
							case '#': st.push_back('#'); break;
							case '(': st.push_back('('); break;
							case '[': st.push_back('['); break;
							case '{': st.push_back('{'); break;
							case ')': error("Found ')' where not expected"); exit(0);
							case ']': error("Found ']' where not expected"); exit(0);
							case '}': st.pop_back(); break;
							}
							break;
				}
			}
			filling+=front;
		}while(st.size()>0);
		return filling;
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
	 * Precondition: read() gives a single or double quote
	 *
	 */
	String readString(char endWith){
		String filling = "";
		trim(endWith);
		if(peek()!='\'' && peek()!='"') error("Cannot read false string literal",true);
		bool isDouble = read()=='"';
		int escape = 0;
		bool doneV = false;
		do{
			char front = read();
			if(escape == 0)
				switch(front){
				case '"':
					if(isDouble){doneV = true; break; }
					else filling+=front;
					break;
				case '\'':
					if(!isDouble){doneV = true; break; }
					else filling+=front;
					break;
				case '\\':
					escape = 1;
					break;
				default:
					filling+=front;
					break;
				}
			else if(escape==1){
				unsigned int n;
				switch(front){
				case '\\':
					filling+='\\';
					escape = 0;
					break;
				case '\r':
					if(peek()=='\n') read();
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
				case 'N':
					escape = 0;
					//TODO implement STRING N
					//							filling+='\N';
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
		}while(!doneV);
		trim(endWith);
		if(done) return filling;
////////		printf("^^ %c %d\n",peek(),peek());
		if(peek()=='"' || peek()=='\'') return filling+readString(endWith);
		return filling;
	}

	String readWhile(const bool (*fun)(char)){
		String st;
		char load;
		do{
			load = read();
			if(!fun(load)){
				break;
			}
			st+=load;
		}while(true);
		write();
		return st;
	}
	String peekUntil(const bool (*fun)(char)){
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
	}

	void error(String a = "Compile error", /*String b="", String c="", String d="",*/ bool end=false){
		cerr << a /*<< b << c << d*/ << "  on line " << readChars.size() << " character " << readChars.back().size() << endl << flush;
		if(end) exit(1);
	}

	bool startsWith(String s){
		return s==peek(s.length());
	}

	bool trimNonLine(char endWith=EOF){
		if(done){
			return true;
		}
		char c;
		do{
			c = read();
			if(c==EOF){
				write();
				done = true;
				return true;
			}
			else if(c==endWith){
				write();
				return false;
			}
			else if(endAtLines && c=='\n'){
				done = true;
				return true;
			}
		}while(c==' ' || c=='\t' || c=='\r');
		write();
		return false;
	}

	bool trimEndOfLine(){
		char c;
		if(done){
			return true;
		}
		do{
			c = read();
			cerr << "Read: " << c << endl;
			if(c==EOF){
				write();
				done = true;
				return true;
			}
		}while(c=='\n');
		done |= endAtLines;
		return endAtLines;
	}

	/**
	 * Returns whether the file has ended
	 */
	bool trim(char endWith/*=EOF*/){
		if(done){
			return true;
		}
		do{
			if(trimNonLine(endWith)){
				return true;
			}
			char c = peek();
			if(c==EOF || (endAtLines && c=='\n')){
				done = true;
				return true;
			}
			if(c==endWith){
				return false;
			}
			if(c=='#' && trimEndOfLine()){
				return true;
			}
			if(c=='/'){
				read();
				char n = peek();
				if(n=='/' && trimEndOfLine()){
					return true;
				}
				else if(n=='*'){
					char prev = ' ';
					char cur = ' ';
					do{
						prev = cur;
						cur = read();
						if(cur==EOF){
							write();
							error("Unclosed /* comment");
							exit(0);
							return true;
						}
					}while(prev!='*' || cur!='/');
					write();
				}
			}
			if(c!='\n') break;
			else read();
		}while(true);
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
			if(done || peek()!=',') return vals;
			else read();
		}
		while(true);
		return vals;
	}
	String getNextName(int endWith){
		//trim();
		if(done) return "";
		char nex = peek();
		if(nex==endWith){ done=true; return ""; }
		if (isalpha(nex) || nex=='$' || nex=='_') {
			String temp = "";
			char tchar;
			do{
				tchar = read();
				if(endWith==tchar){
					write();
					done = true;
					return temp;
				}
				if(isalnum(tchar) || tchar=='$' || tchar=='_')	temp+=tchar;
				else break;
			}while(true);
			if(!(tchar==' ' || tchar=='\n' || tchar == '\t')) write();
			if(endAtLines && tchar=='\n') done = true;
			return temp;
		}
		return "";
	}
	oobject* readNumber(char endWith){
		String hi;
		char tchar;
		bool decimal = false;
		int pos = 0;
		int base = 10;
		bool te = false;
		do{
			if(done) break;
			tchar = peek();
			if(tchar==endWith){
				done=true;
				break;
			}
			else
				read();
			if(tchar=='.' || (base<15 && (tchar=='e' || tchar=='E'))){
				hi+=tchar; decimal=true; te = true;
				pos++; continue;
			}
			else if(pos==0 && tchar=='0'){
				//hi+=tchar;
				base=-1;
			}
			else if(pos==1 && base==-1){
				if(tchar=='x' || tchar=='X'){
					base=16;
					//	hi.pop_back();
				}
				else if(tchar=='b' || tchar=='B'){
					base = 2;
					//hi.pop_back();
				}
				else if(tchar>='0' && tchar<='7'){
					base = 8;
					hi+=tchar;
				}
				else break;
			}
			else if(isdigit(tchar) || (tchar>='a' && tchar<='f')  || (tchar>='A' && tchar<='F')){
				hi+=tchar;
			}
			else if(te && tchar=='-') hi+=tchar;
			else break;
			pos++;
			te = false;
		}while(true);
		if(base==-1) base=10;
		//hi.pop_back();
		write();
		if(decimal){
			return new odec(strtod(hi.c_str(),NULL));
		}
		else{
			return new oint(strtol(hi.c_str(),NULL, base));
		}
	}
	String getNextOperator(char endWith){
		if (isOperator(peek())) {
			String temp = readWhile(isOperator);
			if(temp.size()==0) temp = readString(endWith);
			bool binop = false;
			for(const auto& a:BINARY_OPERATORS){
				if(a==temp){
					binop = true;
					break;
				}
			}
			if(binop){
				trim(endWith);
				return temp;
			}
			else for(unsigned int i = 0; i<temp.size(); ++i) write();
			return "";
		}
		return "";
	}

};


Expression* getIndex(Stream* f, Expression* toIndex, std::vector<Expression*>& stack){
	if(stack.size()==0){
		//TODO HANDLE APPEND OPERATORS
		f->error("Append operators not implemented yet",true);
	}
	if(stack.size()==1 && stack[0]!=NULL){
		Expression* temp = stack[0];
		stack.clear();
		return new E_INDEXER(toIndex,temp);
	}
	else{
		//TODO allow for a[::,2] or a[:,:]
		Expression* start = NULLV, *end=NULLV,*step=NULLV;
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
		stack.clear();
		E_SLICE* e = new E_SLICE(start,end,step);
		return new E_INDEXER(toIndex,e);
	}
}
#endif /* O_STREAM_HPP_ */
