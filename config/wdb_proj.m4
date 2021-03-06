#
#   wdb - weather and water data storage
#
#   Copyright (C) 2007 met.no
#   
#   Contact information:
#   Norwegian Meteorological Institute
#   Box 43 Blindern
#   0313 OSLO
#   NORWAY
#   E-mail: wdb@met.no
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
#   MA  02110-1301, USA
#
#   Checks for the Presence of PROJ.4
#

AC_DEFUN([WDB_PROJ_CHECK],
[
# Set up option
AC_ARG_WITH([proj],
	    AS_HELP_STRING([--with-proj=PATH], [Specify the directory in which proj is installed (by default, configure uses the environment variable LDFLAGS). If set, configure will add PATH/include to CPPFLAGS and PATH/lib to LDFLAGS]),
	    [ac_proj_path="$withval"],
            [])

# Add path if given
if test "$ac_proj_path" != ""; then
   PROJ_CPPFLAGS="-I$ac_proj_path/include"
	PROJ_LDFLAGS="-L$ac_proj_path/lib"
else
	for ac_proj_path_tmp in /usr /usr/local /opt /opt/proj; do
     	if test -f "$ac_proj_path_tmp/include/proj_api.h"; then
         PROJ_CPPFLAGS="-I$ac_proj_path_tmp/include"
        	PROJ_LDFLAGS="-L$ac_proj_path_tmp/lib"
         break;
      fi
   done
fi

# Set up environment

OLD_CPPFLAGS="$CPPFLAGS $PROJ_CPPFLAGS"
OLD_LDFLAGS="$LDFLAGS $PROJ_LDFLAGS"
CPPFLAGS="$PROJ_CPPFLAGS"
LDFLAGS="$PROJ_LDFLAGS"

# Search for the Library
# automatically adds -lproj to the LIBS variable
#AC_LANG_PUSH([C++])
AC_SEARCH_LIBS([pj_init_plus], 
		[proj],[LDFLAGS=$OLD_LDFLAGS; CPPFLAGS=$OLD_CPPFLAGS], 
		[AC_MSG_ERROR([
-------------------------------------------------------------------------
    Unable to link with proj. If the library is installed, make sure 
    -L(PROJ_PATH) is in your LDFLAGS, or specify the path in which proj 
    is installed with --with-proj=PATH

    LDFLAGS: $LDFLAGS
-------------------------------------------------------------------------
])	
		]
)

#AC_LANG_POP([C++])

])
