/*
 * E_EOF.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_EOF_HPP_
#define E_EOF_HPP_

#include "../O_Expression.hpp"

class E_EOF : public Expression{
	public:
		E_EOF(){};
		~E_EOF(){};
		bool writeBinary(FILE* f){
			return writeByte(f, T_EOF);
		}
		bool readBinary(FILE* f){
			byte c;
			if(readByte(f,&c)) return true;
			return c!=T_EOF;
		}
		Token getToken(){
			return T_EOF;
		}
		ostream& write(ostream& f){
			return f << "E_EOF()";
		}
};


#endif /* E_EOF_HPP_ */
