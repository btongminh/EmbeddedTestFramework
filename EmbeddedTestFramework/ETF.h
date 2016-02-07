/*
 Copyright (c) 2016 Bryan Tong Minh

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * EmbeddedTestFramework - Test framework suitable for embedded devices with
 * relative large flash memories and moderate RAM memory.
 */

#ifndef ETF_H_
#define ETF_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Imply the shorter types if the longer types are defined
#ifdef ETF_LONGLONG_SUPPORT
	#ifndef ETF_LONG_SUPPORT
		#define ETF_LONG_SUPPORT
	#endif
#endif
#ifdef ETF_DOUBLE_SUPPORT
	#ifndef ETF_FLOAT_SUPPORT
		#define ETF_FLOAT_SUPPORT
	#endif
#endif
#ifdef ETF_FLOAT_SUPPORT
	#include <float.h>
	#include <math.h>
#endif

/**
 * Mixed data type for unit test results
 */
typedef union
{
	unsigned int uval;
	int ival;
	const void* pval;
#ifdef ETF_LONG_SUPPORT
	unsigned long ulval;
	long lval;
#endif
#ifdef ETF_FLOAT_SUPPORT
	float fval;
#endif
#ifdef ETF_LONGLONG_SUPPORT
	unsigned long long ullval;
	long long llval;
#endif
#ifdef ETF_DOUBLE_SUPPORT
	double dval;
#endif
} ETFMixedType;



/**
 * Unit test result type
 */
typedef struct
{
	unsigned int line;
	const char* lineName;
	const char* functionName;
	ETFMixedType expected;
	ETFMixedType actual;
} ETFResultType;

typedef void (*ETFTestFunction)( ETFResultType* );
typedef void (*ETFResultCallback)( bool, const char*, const ETFResultType* );

/**
 * Define a test suite
 */
#define ETF_TESTSUITE( functionName, callbackFunction, functionBody ) \
bool functionName( void ) { \
	const static char strTest[] = "Test "; \
	const static char strPassed[] = " passed\n"; \
	const static char strFailed[] = " failed\n"; \
	const static char strAssertionFailed[] = "Assertion failed on line "; \
	static char message[128] = {0}; \
	\
	static int taskCount = 0; \
	ETFTestFunction testFunction = NULL; \
	static ETFResultType result = { .line = 0 }; \
	const ETFResultCallback callback = callbackFunction; \
	\
	static int testID = __COUNTER__ + 1; \
	switch ( testID ) \
	{ \
		case __COUNTER__: \
			functionBody \
			testID = -1; \
		case -1: \
		default: \
			return true; \
	} \
}

/**
 * Run a test suite until finished
 */
#define ETF_RUNTESTSUITE( testSuite )  do {} while ( testSuite() != true )

/**
 * Helper macros to define a static string variable containing the test case name
 */
#define ETF_FUNCTIONVAR( functionName_ ) result.functionName = #functionName_;

/**
 * Run a test/task
 */
#define ETF_RUNTEST( test, task, count )  do { \
			testID = __COUNTER__ + 1; \
			taskCount = 0; \
		case __COUNTER__: \
			testFunction = task; \
			if ( testFunction != NULL && taskCount < count && result.line == 0 ) { \
				ETF_FUNCTIONVAR( task ); \
				testFunction( &result ); \
				taskCount++; \
				return false; \
			} \
			testID = __COUNTER__ + 1; \
			testFunction = test; \
			if ( testFunction != NULL && result.line == 0 ) { \
				ETF_FUNCTIONVAR( test ); \
				testFunction( &result ); \
			} \
			return false; \
		case __COUNTER__: ; \
			strcpy( message, strTest ); \
			strncat( message, result.functionName, sizeof( message ) - strlen( message ) - 1 ); \
			if ( result.line == 0 ) { \
				strncat( message, strPassed, sizeof( message ) - strlen( message ) - 1 ); \
			} else { \
				strncat( message, strFailed, sizeof( message ) - strlen( message ) - 1 ); \
				strncat( message, strAssertionFailed, sizeof( message ) - strlen( message ) - 1 ); \
				strncat( message, result.lineName, sizeof( message ) - strlen( message ) - 1 ); \
			} \
			if ( callback != NULL ) { \
				callback( result.line == 0, message, &result ); \
			} } while ( 0 )


/**
 * Define a test case
 */
#define ETF_TESTCASE( testCaseName ) \
void testCaseName( ETFResultType* result )

/**
 * Helper macros to define a static string variable containing the line number
 */
#define ETF_LINEVAR2( lineNumber ) const static char line##lineNumber[] = #lineNumber; result->lineName = line##lineNumber;
#define ETF_LINEVAR( lineNumber ) ETF_LINEVAR2( lineNumber )

/**
 * Mark an assertion failure
 */
#define ETF_ASSERT_FAILED() do { \
	result->line = __LINE__; \
	ETF_LINEVAR( __LINE__ ); \
	return; \
	} while ( 0 )

/**
 * Assert that two ints are equal
 */
#define ETF_ASSERT_IEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.ival = act; \
		result->expected.ival = exp; \
	} } while ( 0 )

/**
 * Assert that two unsigned ints are equal
 */
#define ETF_ASSERT_UEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.uval = act; \
		result->expected.uval = exp; \
	} } while ( 0 )

/**
 * Assert that two pointers are equal
 */
#define ETF_ASSERT_PEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.pval = act; \
		result->expected.pval = exp; \
	} } while ( 0 )

/**
 * Assert that two longs are equal
 */
#define ETF_ASSERT_LEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.lval = act; \
		result->expected.lval = exp; \
	} } while ( 0 )

/**
 * Assert that unsigned longs are equal
 */
#define ETF_ASSERT_ULEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.ulval = act; \
		result->expected.ulval = exp; \
	} } while ( 0 )

/**
 * Assert two floats are equal within tolerance
 */
#define ETF_ASSERT_FEQUAL( act, exp, tol ) do { \
	if ( fabsf( act - exp ) > tol ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.fval = act; \
		result->expected.fval = exp; \
	} } while ( 0 )

/**
 * Assert that two long longs are equal
 */
#define ETF_ASSERT_LLEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.llval = act; \
		result->expected.llval = exp; \
	} } while ( 0 )

/**
 * Assert that two unsigned long longs are equal
 */
#define ETF_ASSERT_ULLEQUAL( act, exp ) do { \
	if ( act != exp ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.ullval = act; \
		result->expected.ullval = exp; \
	} } while ( 0 )

/**
 * Assert that two doubles are equal within tolerance
 */
#define ETF_ASSERT_DEQUAL( act, exp, tol ) do { \
	if ( fabs( act - exp ) > tol ) { \
		ETF_ASSERT_FAILED(); \
		result->actual.dval = act; \
		result->expected.dval = exp; \
	} } while ( 0 )

#endif /* ETF_H_ */
