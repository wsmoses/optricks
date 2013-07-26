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
	T_POS = 8,
	T_NEG = 9,
	T_NOT = 10,
	T_REF = 11,
	T_DEREF = 12,
	T_BNOT = 13,
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
	T_INDEXER = 24
};

#endif /* O_TOKEN_HPP_ */
