/*
 * WeatherSymbolDataBuffer.h
 *
 *  Created on: Feb 28, 2014
 *      Author: borgem
 */

#ifndef __WEATHERSYMBOL_H__
#define __WEATHERSYMBOL_H__

#include <map>
#include <iostream>
#include <float.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <weather_symbol/Factory.h>

namespace wdb2ts {

struct SymbolDataElement: virtual public weather_symbol::WeatherData
{
	float thunderProbability;
	float fogCover;
	float maxTemperature_6h;
	float minTemperature_6h;
	weather_symbol::Code weatherCode;
	std::string provider;
	boost::posix_time::ptime from;
	boost::posix_time::ptime to;

	SymbolDataElement() :
			thunderProbability( FLT_MAX ), fogCover( FLT_MAX ), maxTemperature_6h(
			FLT_MAX ), minTemperature_6h( FLT_MAX ), weatherCode(
					weather_symbol::Error )
	{
	}

	SymbolDataElement( const weather_symbol::WeatherData &wd ) :
			weather_symbol::WeatherData( wd ), thunderProbability( FLT_MAX ), fogCover(
			FLT_MAX ), maxTemperature_6h( FLT_MAX ), minTemperature_6h(
			FLT_MAX ), weatherCode( weather_symbol::Error )
	{
	}

	bool valid(bool ignorePrecipitation=false) const
	{
		return (ignorePrecipitation || precipitation != FLT_MAX) && totalCloudCover != FLT_MAX
						&& temperature != FLT_MAX;
	}

	int hours() const
	{
		return (to - from).hours();
	}

	/**
	 * Set all data that is valid for a specific timeperiode to
	 * an invalid state if i does'nt match the timeperiod.
	 *
	 * At the moment this only applies to maxTemperature_6h and
	 * minTemperature_6h.
	 */
	void clean( int hours )
	{
		if( hours != 6 ){
			maxTemperature_6h = FLT_MAX;
			minTemperature_6h = FLT_MAX;
		}
	}
};

std::ostream&
operator<<( std::ostream &o, const wdb2ts::SymbolDataElement &data );

class WeatherSymbolDataBuffer
{
	WeatherSymbolDataBuffer( const WeatherSymbolDataBuffer & );
	WeatherSymbolDataBuffer& operator=( const WeatherSymbolDataBuffer & );

public:

	typedef std::map<boost::posix_time::ptime, SymbolDataElement> SymbolData;
	typedef SymbolData::const_iterator const_iterator;
	typedef SymbolData::const_reverse_iterator const_reverse_iterator;
	//typedef std::pair<const_iterator, const_iterator> slice_iterator;

	//The SliceIterator returns a slice into the data for a time interval.
	//The time interval is defined with from_time and to_time.
	//There is two iterators to the beginning.
	//first is defined at <from_time, to_time]
	//start is defined at [from_time, to_time]
	struct SliceIterator
	{
		const SymbolData &container;
		const_iterator start;
		const_iterator first;
		const_iterator second;
		int timestep;

		SliceIterator( const SymbolData &container ) :
				container( container ), first( container.end() ), second( first ), start(
						container.begin() ), timestep( INT_MAX )
		{
		}

//		SliceIterator( const SymbolData &container, const_iterator first,
//				const_iterator second, int timestep ) :container( container ),
//				first( first ), second( second ), beforeFirst( first-- ), timestep(timestep)
//		{
//		}

		SliceIterator( const SymbolData &container, const_iterator start_,
				const_iterator end, int timestep ) :
				container( container ), start( start_ ), first( ++start_ ),
				second( end ), timestep( timestep )
		{
		}

//		operator bool() const
//		{
//			return first != container.end();
//		}
		bool operator!() const
		{
			return first == container.end();
		}
	};
	typedef SliceIterator slice_iterator;

	WeatherSymbolDataBuffer();
	WeatherSymbolDataBuffer( int size );

	void clear();

	void add( const boost::posix_time::ptime &time,
			const SymbolDataElement &data );
	void updateWeatherSymbol( const boost::posix_time::ptime &time,
			const weather_symbol::Code &symbol );

	/**
	 * hasThunder
	 * Check if any symbols generated for the base data has thunder.
	 * If an error occur an exception is thrown.
	 * @throws std::logic_error.
	 */
	bool hasThunder( const boost::posix_time::ptime &time, int hours ) const;

	/**
	 * hasThunder
	 * Check if any symbols generated for the base data has thunder.
	 * If an error occur an exception is thrown.
	 * @throws std::logic_error.
	 */
	bool hasThunder( const slice_iterator &slice ) const;

	slice_iterator slice( const boost::posix_time::ptime &from,
			const boost::posix_time::ptime &to ) const;
	slice_iterator slice( int hours ) const;
	slice_iterator slice( const boost::posix_time::ptime &from,
			int hours ) const;

	const_iterator begin() const
	{
		return data_.begin();
	}
	const_iterator end() const
	{
		return data_.end();
	}

	const_reverse_iterator rbegin() const
	{
		return data_.rbegin();
	}
	const_reverse_iterator rend() const
	{
		return data_.rend();
	}

	std::ostream& print( std::ostream &o, const slice_iterator &it ) const;

	friend std::ostream&
	operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer &data );

	friend std::ostream&
	operator<<( std::ostream &o,
			const wdb2ts::WeatherSymbolDataBuffer::slice_iterator &it );

private:
	size_t size_;
	SymbolData data_;
};

std::ostream&
operator<<( std::ostream &o, const wdb2ts::WeatherSymbolDataBuffer &data );

std::ostream&
operator<<( std::ostream &o,
		const wdb2ts::WeatherSymbolDataBuffer::slice_iterator &it );

namespace WeatherSymbolGenerator {

void init();

std::string symbolName( weather_symbol::Code code );

SymbolDataElement
computeWeatherSymbolData( WeatherSymbolDataBuffer &data, int hours,
		int &timestep );

SymbolDataElement
computeWeatherSymbol( WeatherSymbolDataBuffer &data, int hours, float precip =
FLT_MAX, float precipMin = FLT_MAX, float precipMax = FLT_MAX );

SymbolDataElement
computeWeatherSymbol( WeatherSymbolDataBuffer &data, int hours,
		weather_symbol::Code weatherCode );

}

}

#endif /* WEATHERSYMBOL_H_ */
