/*
 * Mangle.hpp
 *
 *  Created on: May 12, 2014
 *      Author: Billy
 */

#ifndef MANGLE_HPP_
#define MANGLE_HPP_

/*
 * http://www.agner.org/optimize/calling_conventions.pdf
 * Page 24 ^
 *
 * http://mentorembedded.github.io/cxx-abi/abi.html#mangling
 */

template<typename T> struct mangleClass;

#ifdef _MSC_VER
#define MANGLE_DEC(clas, gcc, win)\
template<> struct mangleClass<clas>{\
	constexpr const char* mangled = win;\
};
#else
#define MANGLE_DEC(clas, gcc, win)\
template<> struct mangleClass<clas>{\
	constexpr const char* mangled = gcc;\
};
#endif

#define MANGLE_T_ER(A, B, q)
//#ifdef _MSC_VER
//#define MANGLE_DEC_T(A, B, gcc, win)\
//#if win == error\
//template<A> struct mangleClass<B>{\
//	constexpr const char* mangled = win;\
//};\
//#endif
//#else
//#define MANGLE_DEC_T(A, B, gcc, win)\
//template<A> struct mangleClass<B>{\
//	constexpr const char* mangled = gcc;\
//};
//#endif

/*
MANGLE_DEC(void, "v",  "X")
MANGLE_DEC(bool, "b", "_N")

MANGLE_DEC(char, "c", "D")
MANGLE_DEC(signed char, "a", "C")
MANGLE_DEC(unsigned char, "h", "E")

MANGLE_DEC(short int, "s", "F")

MANGLE_DEC(unsigned short int, "t", "G")
MANGLE_DEC(int, "i", "H")
MANGLE_DEC(unsigned int, "j", "I")
MANGLE_DEC(long int, "l", "J")
MANGLE_DEC(unsigned long int, "m", "K")
MANGLE_DEC(long long, "x", "_J")
MANGLE_DEC(unsigned long long, "y", "_K")
MANGLE_DEC(wchar_t, "w", "_W")
MANGLE_DEC(float, "f", "M")
MANGLE_DEC(double, "d", "N")
MANGLE_DEC(long double, "e", "O")
MANGLE_DEC(__complex__ float, "Cf", error)
MANGLE_DEC(__complex__ double, "Cd", error)
MANGLE_DEC(__m64, "Dv2_i", "T__m64@@")
MANGLE_DEC(__m128, "Dv4_f", "T__m128@@")
MANGLE_DEC(__m128d, "Dv2_d", "U__m128d@@")
MANGLE_DEC(__m128i, "Dv2_x", "T__m128i@@")
MANGLE_DEC(__m256, "Dv8_f", "T__m256@@")
MANGLE_DEC(__m256d, "Dv4_d", "U__m256d@@")
MANGLE_DEC(__m256i, "Dv4_x", "T__m256i@@")

MANGLE_DEC_T(typename A, const A, mangleClass<A>::mangled, mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A*, "P"+mangleClass<A>::mangled, "PEA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, const A*, "PK"+mangleClass<A>::mangled, "PEB"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, volatile A*, "PV"+mangleClass<A>::mangled, "PEC"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, const volatile A*, "PVK"+mangleClass<A>::mangled, "PED"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A* const, "P"+mangleClass<A>::mangled, "QEA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A* volatile, "P"+mangleClass<A>::mangled, "REA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A* const volatile, "P"+mangleClass<A>::mangled, "SEA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, const A* const, "PK"+mangleClass<A>::mangled, "QEB"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A* __restrict, "P"+mangleClass<A>::mangled, "PEIA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A&, "R"+mangleClass<A>::mangled, "AEA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A&&, "O"+mangleClass<A>::mangled, error)
MANGLE_DEC_T(typename A, const A&, "RK"+mangleClass<A>::mangled, "AEB"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, volatile A&, "RV"+mangleClass<A>::mangled, "AEC"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, const volatile A&, "RVK"+mangleClass<A>::mangled, "AED"+mangleClass<A>::mangled)
//global object...?
MANGLE_DEC_T(typename A, A[ ], error, "PA"+mangleClass<A>::mangled)
//global object...?
MANGLE_DEC_T(typename A, A[][8], error, "PAY07"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A[][16][5], error, "PAY1BA@"+mangleClass<A>::mangled)
//function parameter...
MANGLE_DEC_T(typename A, A[ ], "P"+mangleClass<A>::mangled, "QEA"+mangleClass<A>::mangled)
//function parameter
MANGLE_DEC_T(typename A, const A[], "PK"+mangleClass<A>::mangled, "QEB"+mangleClass<A>::mangled)
//function parameter
MANGLE_DEC_T(typename A, A[][8], "PA8_"+mangleClass<A>::mangled, "QEAY07"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A[][16][8], "PA16_A5_"+mangleClass<A>::mangled, "QEAY1BA@4"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A near *, error, "PA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A far *, error, "PE"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A huge *, error, "PI"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A _seg *, error, error)
MANGLE_DEC_T(typename A, A near &, error, "AA"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A far &, error, "AE"+mangleClass<A>::mangled)
MANGLE_DEC_T(typename A, A huge &, error, "AI"+mangleClass<A>::mangled)
*/

//MANGLE_DEC_T(typename A, ty, const A*, "PK"+mangleClass<A>::mangled, "QEB"+mangleClass<A>::mangled)

/*
 * VARARGS
	MANGLE_DEC(..., "z", "Z")
 */
#undef MANGLE_DEC
#endif /* MANGLE_HPP_ */
