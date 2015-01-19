/*
 * WeatherSymbolDataBuffer.h
 *
 *  Created on: Feb 28, 2014
 *      Author: borgem
 */

#ifndef __WEATHERSYMBOL_H__
#define __WEATHERSYMBOL_H__

#include <map>
#include <float.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <weather_symbol/Factory.h>

namespace wdb2ts {

struct SymbolDataElement
		: virtual public weather_symbol::WeatherData
{
	float thunderProbability;
	float fogCover;
	float maxTemperature;
	float minTemperature;
	weather_symbol::Code weatherCode;
	std::string provider;
	boost::posix_time::ptime from;

	SymbolDataElement()
		: thunderProbability( FLT_MAX ), fogCover( FLT_MAX ),
		  maxTemperature( FLT_MAX ), minTemperature( FLT_MAX ),
		  weatherCode( weather_symbol::Error ){}

	SymbolDataElement( const weather_symbol::WeatherData &wd )
		: weather_symbol::WeatherData( wd ), thunderProbability( FLT_MAX ),
		  fogCover( FLT_MAX ), maxTemperature( FLT_MAX ), minTemperature( FLT_MAX ),
		  weatherCode( weather_symbol::Error ){}

	bool valid() const {
		return precipitation != FLT_MAX && totalCloudCover != FLT_MAX && temperature != FLT_MAX;
	}
};

std::ostream&
operator<<( std::ostream &o, const wdb2ts::SymbolDataElement &data);


class WeatherSymbolDataBuffer
{
	WeatherSymbolDataBuffer( const WeatherSymbolDataBuffer &);
	WeatherSymbolDataBuffer& operator=(const WeatherSymbolDataBuffer &);

public:
	typedef std::map<boost::posix_time::ptime, SymbolDataElement > SymbolData;
	typedef SymbolData::const_iterator const_iterator;
	typedef SymbolData::const_reverse_iterator const_reverse_iterator;
	typedef std::pair<const_iterator, const_iterator> slice_iterator;

	WeatherSymbolDataBuffer();
	WeatherSymbolDataBuffer( int size );

	void clear();

	void add( const boost::posix_time::ptime &time, const SymbolDataElement &data );
	slice_iterator slice( const boost::posix_time::ptime &from,  boost::posix_time::ptime &to, int &timestep )const;
	slice_iterator slice( int hours, int &timestep )const;

	const_iterator begin()const { return data_.begin(); }
	const_iterator end()const { return data_.end(); }

	const_reverse_iterator rbegin()const { return data_.rbegin(); }
	const_reverse_iterator rend()const { return data_.rend(); }

	std::ostream&  print( std::ostream &o, const slice_iterator &it )const;

	friend
	std::ostream&
	operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer &data);

	friend
	std::ostream&
	operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer::slice_iterator &it );

private:
	int size_;
	SymbolData data_;
};

std::ostream&
operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer &data);

std::ostream&
operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer::slice_iterator &it );


namespace  WeatherSymbolGenerator {

void init();

std::string symbolName( weather_symbol::Code code );

SymbolDataElement
computeWeatherSymbolData( const WeatherSymbolDataBuffer &data, int hours );

SymbolDataElement
computeWeatherSymbol( const WeatherSymbolDataBuffer &data, int hours, float precip=FLT_MAX, float precipMin=FLT_MAX, float precipMax=FLT_MAX );

SymbolDataElement
computeWeatherSymbol( const WeatherSymbolDataBuffer &data, int hours, weather_symbol::Code weatherCode );

}



}

#endif /* WEATHERSYMBOL_H_ */
