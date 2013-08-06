#ifndef O_STREAM_HPP_
#define O_STREAM_HPP_

const bool isWhitespace(const char a){
	return a==' ' || a=='\n' || a=='\t' || a=='\r';
}

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
public:
	char last;
	unsigned int lineCount, charCount, prev;
	bool start;
	bool done;
	bool endAtLines;
	Stream(FILE* a, bool lines=false){
		done = false;
		endAtLines = lines;
		start = true;
		f = a;
		prev = lineCount = 0;
		last = 0;
		charCount = 0;
	}
	void write(char c){
		done = false;
		if(c=='\n'){ lineCount--; charCount = prev; }
		else charCount--;
		cache.push_back(c);
	}
	char read(){
		if(done) error("Already done reading file");
		char c;
		if(cache.size()==0){
			if(readChar(f,&c)){
				error("Error reading from file: ", ""+c);
				exit(0);
			}
		}
		else{
			c = cache.back();
			cache.pop_back();
		}
		if(c=='\n'){ lineCount++; prev = charCount; charCount = 0;}
		else charCount++;
		last = c;
		return c;
	}
	String readRecursive(){
		String filling = "";
		std::vector<char> st;
		do{
			char front = read();
			if(st.size()==0){
				if(front==EOF){ write(front); return filling; }
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

	String readUntil(char c, char b){
		String st;
		char load;
		do{
			load = read();
			if(load==EOF || load==c || load==b){
				break;
			}
			st+=load;
		}while(true);
		write(load);
		//trim();
		return st;
	}
	String peekUntil(char c, char b){
		String st;
		char load;
		do{
			load = read();
			if(load==EOF || load==c || load==b){
				break;
			}
			st+=load;
		}while(true);
		write(load);
		if(!returnString(f, st)){
			error("Could not return string to stream in peekString()");
			exit(0);
		}
		return st;
	}
	String readUntil(char c){
		String st;
		char load;
		do{
			load = read();
			if(load==EOF || load==c){
				break;
			}
			st+=load;
		}while(true);
		write(load);
		//trim();
		return st;
	}
	String peekUntil(char c){
		String st;
		char load;
		do{
			load = read();
			if(load==EOF || load==c){
				break;
			}
			st+=load;
		}while(true);
		write(load);
		if(!returnString(f, st)){
			error("Could not return char to stream in peekUntil()");
			exit(0);
		}
		return st;
	}
	String read(unsigned int len){
		String c;
		char temp;
		for(unsigned int i = 0; i<len;i++){
			temp = read();
			if(temp==EOF){
				break;
			}
			c+=temp;
		}
		write(temp);
		//trim();
		return c;
	}
	void writeString(String a){
		for(int i = a.length()-1; i>=0; i--){
			write((char)a[i]);
		}
	}
	String peek(unsigned int len){
		String c;
		char temp;
		for(unsigned int i = 0; i<len;i++){
			temp = read();
			if(temp==EOF){
				break;
			}
			c+=temp;
		}
		write(temp);
		writeString(c);
		return c;
	}
	char peek(){
		if(done) error("Already done reading file");
		if(cache.size()>0) return cache.back();
		char c;
		if(readChar(f,&c)){
			error("Error reading from file");
			exit(0);
		}
		cache.push_back(c);
		last = c;
		return c;
	}

	void error(String a = "Compile error", /*String b="", String c="", String d="",*/ bool end=false){
		cerr << a /*<< b << c << d*/ << "  on line " << lineCount << " character " << charCount << endl << flush;
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
				write(c);
				done = true;
#if DEBUG > 1
				level--;
				indent();
				cerr << "Done trimNonLine(char endWith=EOF) (stream) - c==EOF => done==true" << endl;
#endif
				return true;
			}
			if(c==endWith){
				write(c);
				return false;
			}
			if(endAtLines && c=='\n'){
				done = true;
				return true;
			}
		}while(c==' ' || c=='\t' || c=='\r');
		write(c);
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
				write(c);
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
							write(cur);
							error("Unclosed /* comment");
							exit(0);
							return true;
						}
					}while(prev!='*' || cur!='/');
					write(cur);
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
		if(nex==endWith){ write(nex); done=true; return ""; }
		if (isalpha(nex) || nex=='$' || nex=='_') {
			String temp = "";
			char tchar;
			do{
				tchar = read();
				if(endWith==tchar){
					write(tchar);
					done = true;
					return temp;
				}
				if(isalnum(tchar) || tchar=='$' || tchar=='_')	temp+=tchar;
				else break;
			}while(true);
			if(!(tchar==' ' || tchar=='\n' || tchar == '\t')) write(tchar);
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
		write(tchar);
		if(decimal){
			return new odec(strtod(hi.c_str(),NULL));
		}
		else{
			return new oint(strtol(hi.c_str(),NULL, base));
		}
	}


};

bool isOperator(int a){
	return  a=='!' || a=='%' || a=='&' ||
			a=='^' || a=='*' || a=='|' ||/* a=='(' || a==')' ||*/
			a=='-' || a=='+' || a=='=' || /*a=='[' || a==']' ||*/ a=='<' ||
			a=='>' || a=='.' || a=='/' || a=='\\' || a=='|' ||
			/*a=='{' || a=='}' ||*/ a=='?';
}
String getNextOperator(Stream* f, char endWith){
	char nex = f->peek();
	if (isOperator(nex)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
		String temp = "";
		int tchar;
		do{
			tchar = f->read();
			if(isOperator(tchar)) temp+=tchar;
			//taken out due to errors with 2*(3+4), if remember why added
			// in first place write why
			//	else if(tchar=='(' || tchar=='[' || tchar=='{'){
			//		temp+=tchar;
			//		break;
			//	}
			else break;
		}while(true);
		f->write(tchar);
		if(temp=="*^" || temp=="<>" || temp == "." || temp==":" || temp=="::" || temp == "->" ||
				temp =="++" || temp == "--" || temp=="%%" ||
				temp==".*" || temp==":*" || temp=="::*"||temp=="->*" || temp=="=>*" ||
				temp == "**" || temp=="^" || temp == "%" || temp == "*" || temp == "/" ||
				temp == "//" || temp == "+" || temp=="-" || temp=="<<" || temp == ">>" ||
				temp== "<<<" || temp==">>>" || temp == "<=" || temp == "<" || temp == ">" ||
				temp == ">=" ||	temp=="==" || temp=="===" || temp == "!=" || temp=="!==" ||
				temp == "&" || temp == "^^" || temp == "|" || temp == "&&" ||
				temp == "||" || temp == "\\." || temp == "+=" || temp == "%=" ||
				temp == "-=" || temp == "*=" || temp=="\\.=" || temp == "\\=" ||
				temp=="/=" || temp=="//=" || temp == "**=" || temp=="^=" ||
				temp=="|=" || temp=="||="|| temp=="&=" || temp=="&&=" || temp=="^^=" ||
				temp=="=" || temp == ":=" || temp == "::=" ||temp == "<<=" ||
				temp==">>="	|| temp=="<<<" || temp==">>>" || temp=="[" || temp=="{" || temp=="("
		){
			f-> trim(endWith);
			return temp;
		}
		else f->writeString(temp);
		return "";
	}
	return "";
}


Expression* getIndex(Stream* f, Expression* toIndex, std::vector<Expression*>& stack){
	if(stack.size()==0){
		//TODO HANDLE APPEND OPERATORS
		f->error("Append operators not implemented yet",true);
	}
	if(stack.size()==1 && stack[0]->getToken()!=T_SEP){
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
