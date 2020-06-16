/*------------------------------------------------------------------------------------------------------------
file		timeutil.cpp
brief		Functions for date and time data.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "timeutil.h"
#include "UtilitiesMPI.h"
#include <ctime>
#include <cstdlib>
#if defined(MSWINDOWS) && !defined(CYGWIN)
  #include <windows.h>
  #include <time.h>
#else
  #include <sys/time.h>
#endif

namespace mtbmpi {


//------------------------------------------------------------------------------
// conversion
//------------------------------------------------------------------------------

void ToTimeValues (
    TimeValues & timeValues,		// output time values
    std::string const & timeStr,	// time in form: HH:MM:SS
    char const separator)		// separator between time parts
{
    timeValues.Clear();
    StrVec tokens;
    ParseTokens( timeStr, tokens, separator, 3 );
    if ( tokens.size() > 0 )					// hour
	timeValues.hour   = std::atoi( tokens[0].c_str() );
    if ( tokens.size() > 1 )					// minute
	timeValues.minute = std::atoi( tokens[1].c_str() );
    if ( tokens.size() > 2 )					// second
	timeValues.second = std::atoi( tokens[2].c_str() );
}

void ToDateValues (
    DateValues & dateValues,		// output date values
    std::string const & dateStr,	// date in form: mm/dd/yy
    char const separator)		// separator between date parts
{
    dateValues.Clear();
    StrVec tokens;
    ParseTokens( dateStr, tokens, separator, 3 );
    if ( tokens.size() > 0 )					// year
	dateValues.year  = std::atol( tokens[0].c_str() );
    if ( tokens.size() > 1 )					// month
    {
	short month = FromString<short>( tokens[1] );
	if ( month < 1 )
	    month = 1;
	if ( month > 12 )
	    month = 12;
	dateValues.month = (TMonth)month;
    }
    if ( tokens.size() > 2 )					// day
	dateValues.day   = std::atoi( tokens[2].c_str() );
}

namespace {

    inline void PrependZeroIf (
	std::string & s,
	short const sizeNeeded = 2)
    {
	char const zero = '0';
	short const numToInsert = sizeNeeded - s.size();
	s.insert( 0, numToInsert, zero );
    }

} // namespace

std::string & ToTimeStr (
    std::string & timeStr,		// output time in form: HH:MM:SS
    TimeValues const & timeValues,	// time values
    char const separator)		// separator between date parts
{
    std::string str = ToString(timeValues.hour);
    PrependZeroIf( str );
    timeStr = str;
    timeStr += separator;
    str = ToString(timeValues.minute);
    PrependZeroIf( str );
    timeStr += str;
    timeStr += separator;
    str = ToString(timeValues.second);
    PrependZeroIf( str );
    timeStr += str;
    return timeStr;
}

std::string ToTimeStr (
    TimeValues const & timeValues,	// time values
    char const separator)		// separator between date parts
{
    std::string str;
    ToTimeStr( str, timeValues, separator );
    return str;
}

std::string & ToDateStr (
    std::string & dateStr,		// output date in form: yyyy/mm/dd
    DateValues const & dateValues,	// date values
    char const separator)		// separator between date parts
{
    std::string str = ToString(dateValues.year);
    PrependZeroIf( str, 4 );
    dateStr = str;
    dateStr += separator;
    str = ToString(dateValues.month);
    PrependZeroIf( str );
    dateStr += str;
    dateStr += separator;
    str = ToString(dateValues.day);
    PrependZeroIf( str );
    dateStr += str;
    return dateStr;
}

std::string ToDateStr (
    DateValues const & dateValues,	// date values
    char const separator)		// separator between date parts
{
    std::string str;
    ToDateStr( str, dateValues, separator );
    return str;
}

//------------------------------------------------------------------------------
//	DateStr
//	Returns a string containing the local date in the format: "yyyy/mm/dd"
//------------------------------------------------------------------------------
std::string DateStr ()
{
    std::string returnString = "0000/00/00";
    std::time_t const tp = std::time (NULL);
    if (tp != (std::time_t)-1)
    {
	static std::size_t const sMaxLen = 11;		// includes NULL
	char s[sMaxLen];				// holds formatted time
	struct std::tm const * const ltp = std::localtime (&tp);
	std::size_t actualLength = std::strftime (s, sMaxLen, "%Y/%m/%d", ltp);
	if ( actualLength > 0 )
	    returnString = s;
    }
    return returnString;
}

//------------------------------------------------------------------------------
//	TimeStr
//	Returns a string containing the local time in the format: "hh:mm:ss"
//	using a 24-hour clock.
//------------------------------------------------------------------------------
std::string TimeStr ()
{
    std::string returnString = "00:00:00";
    std::time_t const tp = std::time (NULL);
    if (tp != (std::time_t)-1)
    {
	static std::size_t const sMaxLen = 9;		// includes NULL
	char s[sMaxLen];				// holds formatted time
	struct std::tm const * const ltp = std::localtime (&tp);
	std::size_t actualLength = std::strftime (s, sMaxLen, "%H:%M:%S", ltp);
	if ( actualLength > 0 )
	    returnString = s;
    }
    return returnString;
}

//------------------------------------------------------------------------------
//	DateTimeStr
//	Returns a string containing the local date and time in the format:
//		"yyyy/mm/dd hh:mm:ss"
//	using a 24-hour clock.
//------------------------------------------------------------------------------
std::string DateTimeStr ()
{
	std::string returnString = DateStr();
	returnString += ' ';	// blank
	returnString += TimeStr();
	return returnString;
}

//------------------------------------------------------------------------------
// MakeDateTimeStamp
// Make a date-timestamp string of the time NOW; e.g., "01-01-2016_12-00-00"
//------------------------------------------------------------------------------
std::string MakeDateTimeStamp (
    std::string const & dateStr,
    std::string const & timeStr,
    char const separatorValues,
    char const separatorFields)
{
    DateValues dateValues;
    ToDateValues( dateValues, dateStr );
    TimeValues timeValues;
    ToTimeValues( timeValues, timeStr );
    std::string stamp = ToDateStr( dateValues, separatorValues );
    stamp += separatorFields;
    stamp += ToTimeStr( timeValues, separatorValues );
    return stamp;
}


} // namespace mtbmpi
