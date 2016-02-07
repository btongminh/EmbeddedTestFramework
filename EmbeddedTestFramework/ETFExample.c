/*
 * ETF.c
 *
 *  Created on: 6 feb. 2016
 *      Author: Bryan
 */

#include <stdio.h>

#include "Inc/ETF.h"

void TestSuiteCallback( bool passed, const char* message, const ETFResultType* result )
{
	printf( message );
}

ETF_TESTCASE( TestCaseA )
{
	ETF_ASSERT_IEQUAL( -1, -1 );
}

static volatile int bCount = 0;
ETF_TESTCASE( TestCaseBTask )
{
	bCount++;
}
ETF_TESTCASE( TestCaseB )
{
	ETF_ASSERT_IEQUAL( bCount, 10 );
}

ETF_TESTCASE( TestCaseC )
{
	ETF_ASSERT_IEQUAL( 1, 1 );
	ETF_ASSERT_IEQUAL( 0, 1 );
}

ETF_TESTSUITE( TestSuite, TestSuiteCallback,
	ETF_RUNTEST( TestCaseA, NULL, 0 );
	ETF_RUNTEST( TestCaseB, TestCaseBTask, 10 );
	ETF_RUNTEST( TestCaseC, NULL, 0 );
);

int main( int argc, char** argv )
{
	ETF_RUNTESTSUITE( TestSuite );
	return 0;
}
