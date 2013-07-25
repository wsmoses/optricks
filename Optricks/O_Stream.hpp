#ifndef O_STREAM_HPP_
#define O_STREAM_HPP_

//class Stream;

bool isWhitespace(char a){
	return a==' ' || a=='\n' || a=='\t' || a=='\r';
}

#include "O_IO.hpp"
#include "O_Expression.hpp"
//#define DEBUG 3
#ifdef DEBUG
unsigned int level = 0;
void indent(){ for(int i = 0; i<level; i++) cerr << "  ";}
#endif

class Stream{
	private:
		FILE* f;
		std::vector<char> cache;
	public:
		unsigned int lineCount, charCount, prev;
		bool start;
		bool done;
		bool endAtLines;
		Stream(FILE* a, bool lines=false){done = false; endAtLines = lines; start = true; f = a; prev = lineCount = 0; charCount = 0; };
		/*Stream(const Stream &s){
			f = s.f;
			done = s.done;
			endAtLines = s.endAtLines;
			lineCount = s.lineCount;
			charCount = s.charCount;
			prev = s.prev;
			start = s.start;
		}*/
		void write(char c){
			done = false;
			#if DEBUG > 2
			indent();
			cerr << "write(char c) (stream): " << c << " ";
			writeChar(stderr,c);
			cerr << endl;
			level++;
			#endif
			if(c=='\n'){ lineCount--; charCount = prev; }
			else charCount--;
			cache.push_back(c);
//			if(returnChar(f,c)){
//				error("Error returning char back to file");
//				exit(0);
//			}
			#if DEBUG > 2
			level--;
			indent();
			cerr << "[";
			for(auto &a:cache){
				writeChar(stderr, a);
				cerr << ", ";
			}
			cerr << "]";
			indent();
			cerr << "Done write(char c) (stream) " << c << " ";
			writeChar(stderr,c);
			cerr << endl;
			#endif
		}
		char read(){
			#if DEBUG > 2
			indent();
			cerr << "read() (stream) - cache size = " << (cache.size());
			cerr << endl;
			level++;
			#endif
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
			#if DEBUG > 2
			level--;
			indent();
			cerr << "Done read() (stream) " << c << " ";
			writeChar(stderr,c);
			cerr << " - advanced to line:" << lineCount << " char:"<<charCount << endl;
			#endif
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
		String readString(char start=EOF){
			String filling = "";
			bool isDouble = (start==EOF)?(read()=='"'):((start=='"') | (read() & 0));
			int escape = 0;
			do{
				char front = read();
				if(escape == 0)
					switch(front){
						case '"':
							if(isDouble){return filling; }
							else filling+=front;
							break;
						case '\'':
							if(!isDouble){return filling; }
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
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							n = 0;
							for(int i = 0; i<3; i++){
								n<<=3;
								n|=readOctDigit();
							}
							escape = 0;
							filling+=n;
							break;
						case 'h':
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
//			if(!returnString(f, a)){
//				error("Could not return string to stream in writeString(): ", "'",a, "'");
//				exit(0);
//			}
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
			return c;
		}
		/*void ungets(String s){
			for(unsigned int i = s.length()-1; i>=0; i--){
				writeChar(f, s[i]);
				charCount--;
			}
		}*/

		void error(String a = "Compile error", /*String b="", String c="", String d="",*/ bool end=false){
			cerr << a /*<< b << c << d*/ << "  on line " << lineCount << " character " << charCount << endl << flush;
			if(end) exit(1);
		}

		bool startsWith(String s){
			return s==peek(s.length());
		}

		bool trimNonLine(char endWith=EOF){
			#if DEBUG > 1
			indent();
			cerr << "trimNonLine(char endWith=EOF) (stream)" << endl;
			level++;
			#endif
			if(done){
				#if DEBUG > 1
				level--;
				indent();
				cerr << "Done trimNonLine(char endWith=EOF) (stream) - done==true" << endl;
				#endif
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
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trimNonLine(char endWith=EOF) (stream) - c==endWith" << endl;
					#endif
					return false;
				}
				if(endAtLines && c=='\n'){
					done = true;
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trimNonLine(char endWith=EOF) (stream) - endAtLines && c=='\\n' => done==true" << endl;
					#endif
					return true;
				}
			}while(c==' ' || c=='\t' || c=='\r');
			write(c);
			#if DEBUG > 1
			level--;
			indent();
			cerr << "Done trimNonLine(char endWith=EOF) (stream) - c=='\n' or nonspace" << endl;
			#endif
			return false;
		}

		bool trimEndOfLine(){
			#if DEBUG > 1
			indent();
			cerr << "trimEndOfLine() (stream)" << endl;
			level++;
			#endif
			char c;
			if(done){
				#if DEBUG > 1
				level--;
				indent();
				cerr << "Done trimEndOfLine() (stream) - done==true" << endl;
				#endif
				return true;
			}
			do{
				c = read();
				cerr << "Read: " << c << endl;
				if(c==EOF){
					write(c);
					done = true;
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trimEndOfLine() (stream) - c==EOF" << endl;
					#endif
					return true;
				}
			}while(c=='\n');
			#if DEBUG > 1
			level--;
			indent();
			cerr << "Done trimEndOfLine() (stream) - c=='\\n'" << endl;
			#endif
			done |= endAtLines;
			return endAtLines;
		}

		/**
		 * Returns whether the file has ended
		 */
		bool trim(char endWith/*=EOF*/){
			#if DEBUG > 1
			indent();
			cerr << "trim(char endWith=EOF) (stream)" << endl;
			level++;
			#endif
			if(done){
				#if DEBUG > 1
				level--;
				indent();
				cerr << "Done trim(char endWith=EOF) (stream) - done==true" << endl;
				#endif
				return true;
			}
			do{
				if(trimNonLine(endWith)){
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trim(char endWith=EOF) (stream) - trimNonLine()==true" << endl;
					#endif
					return true;
				}
				char c = peek();
				if(c==EOF || (endAtLines && c=='\n')){
					done = true;
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trim(char endWith=EOF) (stream) - now done from ";
					cerr << (c==EOF)?"EOF":"endAtLines && c=='\\n'";
					cerr << endl;
					#endif
					return true;
				}
				if(c==endWith){
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trim(char endWith=EOF) (stream) - c==endWith==(int)" << endWith << "==(char)";
					if(endWith=='\n')cerr << "\\n";
					else writeChar(stderr, endWith);
					cerr << endl;
					#endif
					return false;
				}
				if(c=='#' && trimEndOfLine()){
					#if DEBUG > 1
					level--;
					indent();
					cerr << "Done trimEndOfLine(char endWith) (stream) - c=='#' && trimEndOfLine()==true" << endl;
					#endif
					return true;
				}
				if(c=='/'){
					read();
					char n = peek();
					if(n=='/' && trimEndOfLine()){
						#if DEBUG > 1
						level--;
						indent();
						cerr << "Done trim(char endWith=EOF) (stream) - c=='//' && trimEndOfLine()==true" << endl;
						#endif
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
			#if DEBUG > 1
			level--;
			indent();
			cerr << "Done trim(char endWith=EOF) (stream) - loop broken (c!='\\n')" << endl;
			#endif
			return false;
		}

		String getNextName(int endWith){
			#if DEBUG > 1
			indent();
			cerr << "getNextName()" << endl;
			level++;
			#endif
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
				//trim();
				#if DEBUG > 1
				level--;
				indent();
				cerr << "Done getNextName() - result='" << temp << "'"<< endl;
				#endif
				return temp;
			}
			#if DEBUG > 1
			level--;
			indent();
			cerr << "Done getNextName() - result=''"<< endl;
			#endif
			return "";
		}
		OBJ_OBJ* readNumber(char endWith){
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
				return new OBJ_DEC(strtod(hi.c_str(),NULL));
			}
			else{
				return new OBJ_INT(strtol(hi.c_str(), NULL, base));
			}
		}


};

bool isOperator(int a){
	return  a=='!' || a=='%' ||
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

Expression* getNextExpression(Stream* f, char endWith,bool opCheck=true);
//TODO implement
Expression* operatorCheck(Stream* f, Expression* exp, char endWith){
	if(f->done || f->trim(endWith)){
		return exp;
	}
	char tchar = f->peek();
	if(tchar=='['){
				f->read();
				std::vector<Expression*> stack;
				Expression* temp = getNextExpression(f,endWith);
				if(temp){
					//TODO HANDLE APPEND OPERATORS
					f->error("Append operators not implemented yet");
				}
				while(true){
					stack.push_back(temp);
					bool comma = true;
					while(!f->done){
						f->trim(endWith);
						char t = f->peek();
						if(t==',' || t==':'){
							f->read();
							if(t==','){
								if(stack.size()==1)
								exp = new E_INDEXER(exp,stack.pop_back());
								else if(stack.size())
							}
							else{
							toReturn->values.push_back(new E_SEP(t));
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
					temp = getNextExpression(f,endWith);
				}
				if(f->done)	f->error("Uncompleted '[' array 2",true);
				char te;
				if((te = f->read())!=']') f->error("Cannot end '[' array with "+te,true);
				return operatorCheck(f, new E_INDEXER(exp, toReturn), endWith);
	}
	else if(tchar=='('){
		//TODO parse function args,  cannot do getNextExpression
		// due to operatorCheck on tuple
		Expression* e = getNextExpression(f, endWith,false);
		if(e->getToken()==T_PARENS){
			E_ARR* temp = new E_ARR( );
			temp->values.push_back(((E_PARENS*)e)->inner);
			free(e);
			e = temp;
		}
		f->trim(endWith);
		return operatorCheck(f, new E_FUNC_CALL(exp, (E_ARR*)e), endWith);
	}
	else if(tchar=='{'){
		f->read();
		f->trim(endWith);
		cerr << " '{' operatorCheck not implemented yet" << endl << flush;
		exit(0);
	}
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

		//		if(tchar=='<' || tchar=='=' || tchar =)
		tchar = f->peek();
		}
	}

	String tmp = getNextOperator(f, endWith);

	if(tmp.length()==0) return exp;
	Expression* fixed;

	if(tmp=="!") {

	}
	else if (tmp == "!="){

	}
	else if (tmp == "<"){
		//equality and check
	}

	else if (tmp == "?"){

	}
	else if (tmp == "." || tmp=="->" || tmp==":" || tmp=="::" || tmp==".*"
			|| tmp==":*" || tmp=="::*"|| tmp=="->*" || tmp=="=>*"){
		Expression* post = getNextExpression(f,endWith,false);
		if(post->getToken()!=T_VAR) f->error("Cannot reference non-variable");
		fixed = operatorCheck(f,(new E_LOOKUP(tmp, exp, (E_VAR*)post)),endWith);

	}
	else if(tmp=="="){
		Expression* post = getNextExpression(f,endWith);
	//	if(tmp->getToken()!=T_VAR)
		fixed = operatorCheck(f,new E_PARENS(new BinaryOperator(tmp, exp, post)),endWith);
	}
	else{
		Expression* post = getNextExpression(f,endWith);
		fixed = operatorCheck(f,(new BinaryOperator(tmp, exp, post))->fixOrderOfOperations(),endWith);
	}
	return fixed;
}

#endif /* O_STREAM_HPP_ */
