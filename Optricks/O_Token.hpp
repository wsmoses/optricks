/*
 * O_Token.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef O_TOKEN_HPP_
#define O_TOKEN_HPP_

enum Token {
	T_EOF = 0,
	T_INT = 1,
	T_EXT = 2,
	T_DEF = 3,
	T_ARR = 4,
	T_VAR = 5,
	T_BINOP = 6,
	T_STR = 7,
	T_UOP = 8,
	T_KEY_FUNC = 14,
	T_FUNC = 15,
	T_DEC = 16,
	T_O_ARR = 17,
	T_SEP = 18,
	T_PARENS = 19,
	T_FUNC_CALL = 20,
	T_LOOKUP = 21,
	T_SLICE = 22,
	T_NULL = 23,
	T_INDEXER = 24,
	T_OOBJECT = 25,
	T_VOID = 26,
	T_CONDITIONAL = 27,
	T_MODULE = 28,
	T_IF = 29,
	T_WHILE = 30,
	T_FOR = 31,
	T_FOREACH = 32,
	T_BLOCK = 33,
	T_DECLARATION = 34,
	T_TERNARY = 35,
	T_SET = 36,
	T_RETURN = 37,
	T_CLASSPROTO = 38,
	T_IMPORT = 39,
	T_GEN = 40,
	T_CLASS
};

#endif /* O_TOKEN_HPP_ */
