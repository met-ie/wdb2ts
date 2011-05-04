#include <cppunit/config/SourcePrefix.h>
#include <exception.h>
#include <iostream>
#include <ptimeutil.h>
#include "ptimeutilTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PtimeUtilTest );

using namespace std;
using namespace miutil;
using namespace boost::posix_time;

void
PtimeUtilTest::
setUp()
{
	// NOOP
}

void
PtimeUtilTest::
tearDown() 
{
	// NOOP
}
      
void 
PtimeUtilTest::
testPtimeFromIsoString()
{
	ptime pt;
	ptime ptTest;
	
	pt = time_from_string("2008-10-18 18:33:22");
	ptTest = ptimeFromIsoString( "2008-10-18 18:33:22" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33:22", pt == ptTest );
	
	pt = time_from_string("2008-10-18 18:33:00");
	ptTest = ptimeFromIsoString( "2008-10-18 18:33" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33", pt == ptTest );
	
	pt = time_from_string("2008-10-18 18:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18 18" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18", pt == ptTest );
	
	pt = time_from_string("2008-10-18 00:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18", pt == ptTest );

	pt = time_from_string("2008-10-18 18:33:22");
	ptTest = ptimeFromIsoString( "2008-10-18T18:33:22" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33:22", pt == ptTest );
		
	pt = time_from_string("2008-10-18 18:33:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:33" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18T18:33", pt == ptTest );
	
	pt = time_from_string("2008-10-18 18:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18", pt == ptTest );
	
	pt = time_from_string("2008-10-18 18:33:22");
	ptTest = ptimeFromIsoString( "20081018T183322" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33:22", pt == ptTest );

	pt = time_from_string("2008-10-18 18:33:22");
	ptTest = ptimeFromIsoString( "20081018183322" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33:22", pt == ptTest );
		
	pt = time_from_string("2008-10-18 18:33:00");
	ptTest = ptimeFromIsoString( "200810181833" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:33", pt == ptTest );
		
	pt = time_from_string("2008-10-18 18:00:00");
	ptTest = ptimeFromIsoString( "2008101818" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18", pt == ptTest );
		
	pt = time_from_string("2008-10-18 00:00:00");
	ptTest = ptimeFromIsoString( "20081018" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18", pt == ptTest );

	
	
	pt = time_from_string("2008-10-18 18:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00+00" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00+00", pt == ptTest );

	pt = time_from_string("2008-10-18 18:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00-00" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00-00", pt == ptTest );
	
	pt = time_from_string("2008-10-18 16:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00+02" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 16:00:00", pt == ptTest );
	
	pt = time_from_string("2008-10-18 16:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00+0200" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00+0200", pt == ptTest );
		
	pt = time_from_string("2008-10-18 15:30:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00+0230" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00+0230", pt == ptTest );

	pt = time_from_string("2008-10-18 20:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00-02" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 16:00:00-02", pt == ptTest );
	
	pt = time_from_string("2008-10-18 20:00:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00-0200" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00-0200", pt == ptTest );
		
	pt = time_from_string("2008-10-18 20:30:00");
	ptTest = ptimeFromIsoString( "2008-10-18T18:00:00-0230" );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "2008-10-18 18:00:00-0230", pt == ptTest );
	
	pt = time_from_string("2007-04-01 09:51:04");
   string rfc("Sun, 01 Apr 2007 09:51:04 GMT");
   ptTest = ptimeFromIsoString( rfc );
	//cerr << pt << " -> (pt) " << ptTest << endl;
	CPPUNIT_ASSERT_MESSAGE( "Sun, 1 Apr 2007 09:51:04 GMT", pt == ptTest );

}
void
PtimeUtilTest::
geologicalLocalTime()
{
   namespace m=miutil;
   namespace b=boost::posix_time;

   b::ptime pt=time_from_string("2007-04-01 09:51:04");
   b::ptime rt;

   rt = m::geologicalLocalTime( pt, 7.4 );
   CPPUNIT_ASSERT_MESSAGE( "utc 0", rt == time_from_string("2007-04-01 09:51:04") );

   rt = m::geologicalLocalTime( pt, 7.6 );
   CPPUNIT_ASSERT_MESSAGE( "utc +1", rt == time_from_string("2007-04-01 10:51:04") );

   rt = m::geologicalLocalTime( pt, -7.6 );
   CPPUNIT_ASSERT_MESSAGE( "utc -1", rt == time_from_string("2007-04-01 08:51:04") );

   rt = m::geologicalLocalTime( pt, 85 );
   CPPUNIT_ASSERT_MESSAGE( "utc +6", rt == time_from_string("2007-04-01 15:51:04") );

   rt = m::geologicalLocalTime( pt, -85 );
   CPPUNIT_ASSERT_MESSAGE( "utc -6", rt == time_from_string("2007-04-01 03:51:04") );
}

void 
PtimeUtilTest::
testRFC1123()
{
	ptime pt = time_from_string("2007-04-01 09:51:04");
   string rfc("Sun, 01 Apr 2007 09:51:04 GMT");

   
   ptime ptTest=rfc1123date( rfc );
   cerr << pt << " -> (pt) " << ptTest << endl;
   CPPUNIT_ASSERT_MESSAGE( "Sun, 1 Apr 2007 09:51:04 GMT", pt == ptTest );
   
   string testRfc=rfc1123date( pt );
   cerr << pt << " -> (pt) " << testRfc << endl;
   CPPUNIT_ASSERT_MESSAGE( "2007-04-01 09:51:04", rfc == testRfc );
}
