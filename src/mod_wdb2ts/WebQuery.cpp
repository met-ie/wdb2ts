#include <list>
#include <sstream>
#include <boost/assign/list_inserter.hpp>
#include <boost/lexical_cast.hpp>
#include <WebQuery.h>
#include <UrlQuery.h>
#include <limits.h>
#include <float.h>

using namespace std;

namespace wdb2ts {

WebQuery
::WebQuery( const std::string &queryToDecode )
{
	*this = decodeQuery( queryToDecode );
}

WebQuery::
WebQuery( )
	: altitude_(INT_MIN)
{
}

WebQuery::
WebQuery( const LocationPointList &locationPoints, int altitude,
		  const boost::posix_time::ptime &from,
		  const boost::posix_time::ptime &reftime,
		  const std::string &dataprovider,
		  bool isPolygon)
	: altitude_( altitude ),
	  from_( from ), reftime_( reftime ), dataprovider_( dataprovider ),
	  isPolygon_( isPolygon )
{
	points = locationPoints;
}


	/**
	 * @exception logic_error
	 */	
WebQuery 
WebQuery::
decodeQuery( const std::string &queryToDecode )
{
	using namespace boost::posix_time;
	ostringstream ost;
	webfw::UrlQuery urlQuery;
	float lat(FLT_MAX);
	float lon(FLT_MAX);
	int alt(INT_MIN);
	boost::posix_time::ptime from;
	boost::posix_time::ptime refTime;
	string dataprovider;
	bool isPolygon( false );
	     
	try { 
		urlQuery.decode( queryToDecode );
		list<string> mustHaveParams;
		string  param;
		
		boost::assign::push_back( mustHaveParams )("long")("lat");
	  
		param = urlQuery.hasParams( mustHaveParams );

		if( urlQuery.hasParam("polygon") ){
			isPolygon = true;
		} else {
			if( ! param.empty() )
				throw logic_error("Missing mandatory parameter  '" + param + "'.");
	   	  
			lat = urlQuery.asFloat( "lat" );
			lon = urlQuery.asFloat( "long" );
			alt = urlQuery.asInt( "alt", INT_MIN );
		}
		
		if( urlQuery.hasParam( "from" ) ) {
			string sfrom=urlQuery.asString( "from", "" );
			if( sfrom=="all" )
				from = boost::posix_time::neg_infin;
			else
				from = urlQuery.asPTime("from", boost::posix_time::not_a_date_time );
		}
		
		//Adjust the fromtime to the nearest hour in the future. 
		if( from.is_not_a_date_time() ) {
			from = second_clock::universal_time();
			from += hours( 1 );
			from = ptime( from.date(), 
					        time_duration( from.time_of_day().hours(), 0, 0, 0 ));
		}
		
		if( urlQuery.hasParam( "refTime" ) ) 
			refTime = urlQuery.asPTime("fromTime", boost::posix_time::not_a_date_time );
		
		if( urlQuery.hasParam( "dataprovider" ) ) 
			dataprovider = urlQuery.asString("dataprovider", "" );

		LocationPointList myPoints;

		if( isPolygon ) {
			LocationPoint::decodePointList( urlQuery.asString( "polygon", "" ), myPoints );
		} else {
			myPoints.push_back( LocationPoint( lat, lon ) );
		}

		return WebQuery( myPoints, alt, from, refTime, dataprovider, isPolygon );
	}
	catch( const std::exception &ex ) {
		ostringstream ost;
		ost << "WebQuery::decodeQuery: Exception, decode query: " << ex.what() << endl 
		<< "Query: " << queryToDecode << endl;
		throw logic_error( ost.str() );
	}
	catch( ... ) {
		ostringstream ost;
		ost << "WebQuery::decodeQuery: Unknown Exception." << endl; 
		throw logic_error( ost.str() );
	}

}


bool 
WebQuery::
isValid()const
{
	if( points.empty() )
		return false;
	
	return true;
}

float
WebQuery::
latitude() const
{
	if( ! points.empty() )
		return points.begin()->latitude();

	return FLT_MAX;
}

float
WebQuery::
longitude()const
{
	if( ! points.empty() )
		return points.begin()->longitude();

	return FLT_MAX;
}

int
WebQuery::
altitude() const
{
	return altitude_;
}



}

