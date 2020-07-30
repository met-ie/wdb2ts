/*
    wdb - weather and water data storage

    Copyright (C) 2007 met.no

    Contact information:
    Norwegian Meteorological Institute
    Box 43 Blindern
    0313 OSLO
    NORWAY
    E-mail: wdb@met.no

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA  02110-1301, USA
*/

#ifndef __METFUNCTIONS_H__
#define __METFUNCTIONS_H__

#include <float.h>
#include <string>

namespace miutil {

/**
 * Compute the relative humidity from temperature and dewpoint temperature.
 *
 * @param temperature The dry bulb temperature in Celcius
 * @param dewPointTemperature The wet bulb temperature in Celcius.
 * @return Relative humidity in percent on success and FLT_MAX on failure.
 */
float
dewPointTemperatureToRelativeHumidity( float temperature, float dewPointTemperature );


/**
 * Compute dew point temperature from temperature
 * and relative humidity.
 *
 * @param temperature air temperature
 * @param relativeHumidity relative humidity in percent.
 * @return The dew point temperature on success and FLT_MAX on failure.
 */
float
dewPointTemperature( float temperature, float relativeHumidity );


/**
 * Compute the Beaufort value and name (in norwegian).
 *
 * | code | Name           | m/s     |knop | Description                                                                                                                              |
 * |:----:|:---------------|:-------:|:---:|:-----------------------------------------------------------------------------------------------------------------------------------------|
 * |     0|Calm            |0,0-0,2  |0-1  |Smoke rises vertically.                                                                                                                   |
 * |     1|Light air       |0,3-1,5  |1-3  |Direction shown by smoke drift but not by wind vanes.                                                                                     |
 * |     2|Light breeze    |1,6-3,3  |4-6  |Wind felt on face; leaves rustle; wind vane moved by wind.                                                                                |
 * |     3|Gentle breeze   |3,4-5,4  |7-10 |Leaves and small twigs in constant motion; light flags extended.                                                                          |
 * |     4|Moderate breeze |5,5-7,9  |11-16|Raises dust and loose paper; small branches moved.                                                                                        |
 * |     5|Fresh breeze    |8,0-10,7 |17-21|Small trees in leaf begin to sway; crested wavelets form on inland waters.                                                                |
 * |     6|Strong breeze   |10,8-13,8|22-27|Large branches in motion; whistling heard in telegraph wires; umbrellas used with difficulty.                                             |
 * |     7|Near gale       |13,9-17,1|28-33|Whole trees in motion; inconvenience felt when walking against the wind.                                                                  |
 * |     8|Gale            |17,2-20,7|34-40|Twigs break off trees; generally impedes progress.                                                                                        |
 * |     9|Strong gale     |20,8-24,4|41-47|Slight structural damage (chimney pots and slates removed).                                                                               |
 * |    10|Storm           |24,5-28,4|48-55|Seldom experienced inland; trees uprooted; considerable structural damage.                                                                |
 * |    11|Violent storm   |28,5-32,6|56-63|Very rarely experienced; accompanied by widespread damage.                                                                                |
 * |    12|Hurricane       |32,7-    |64-  |Devastation.                                                                                                                              |
 *
 * \param[in] mps The wind in meter per second.
 * \param[out] name The beaufort name in norwegian.
 * \return The Beaufort code as a string. If mps is FLT_MAX an empty string is returned and name is also set to an empty string.
 */

std::string
toBeaufort( float mps, std::string &name );



/**
 * Compute the wind direction name.
 * Direction is given in degrees
 *
 * |name|Direction  |
 * |:--:|:----------|
 * |N   |337.5-22.5 |
 * |NE  |22.5-67.5  |
 * |E   |67.5-112.5 |
 * |SE  |112.5-157.5|
 * |S   |157.5-202.5|
 * |SW  |202.5-247.5|
 * |W   |247.5-292.5|
 * |NW  |292.5-337.5|
 *
 * \param dd Wind direction in degrees.
 * \return The name. If dd is FLT_MAX an empty string is returned.
 */
std::string
windDirectionName( float dd );


/**
 * Return the SYMBOLNAME given an symbolid.
 * \note this is not used anymore. Use the methods in the weather symbol library.
 */
std::string
symbolidToName( int id );


}

#endif /* METFUNCTIONS_H_ */
