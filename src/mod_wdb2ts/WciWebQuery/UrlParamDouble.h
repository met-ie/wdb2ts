/*
    wdb - weather and water data storage

    Copyright (C) 2008 met.no

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

#ifndef __URLPARAMDOUBLE_H__
#define __URLPARAMDOUBLE_H__

/**
 * @addtogroup wdb2ts
 * @{
 * @addtogroup mod_wdb2ts
 * @{
 */
/**
 * @file
 * This file contains the UrlParamFloat class definition.
 */

// PROJECT INCLUDES
#include <UrlParam.h>
// SYSTEM INCLUDES
//

namespace wdb2ts
{

/**
 * Decoding Class for Floating Points Parameters from URI
 * 
 * @see UrlParam 
 */
class UrlParamDouble : public UrlParam
{
public:
	/**
	 * Constructor with default value
	 * @param defVal	The default value used for floating points
	 */
    UrlParamDouble( double defVal = DBL_MIN );
    UrlParamDouble( int protocol, double defVal = DBL_MIN );

    virtual void clean();
    /**
     * Return the value
     * @return the value of the UrlParam
     */
    double value()const { return value_; }
    /**
     * Decode the string
     * @param toDecode	Input string to decode
     * @exception logic_error
     */
    virtual void decode( const std::string &toDecode );

private:
	/// The floating point value
    double value_;
    
};

} // namespace

/**
 * @}
 *
 * @}
 */

#endif /*URLPARAMFLOAT_H_*/
