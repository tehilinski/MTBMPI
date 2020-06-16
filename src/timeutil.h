/*! ----------------------------------------------------------------------------------------------------------
@file		timeutil.h
@brief		Functions for date and time data.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_timeutil_h
#define INC_mtbmpi_timeutil_h

// ----------------------------------------------------------------------------
// Functions:
//   DateStr		Gets local date formatted as a string as "YYYY/MM/DD"
//   TimeStr		Gets local time formatted as a string as "HH:MM:SS"
//   DateTimeStr	Gets local date and time formatted as a string as "YYYY/MM/DD HH:MM:SS"
//   MakeDateTime	Makes a date-time string pair.
//   ToTimeValues	Convert TimeStr to TimeValues.
//   ToDateValues	Convert DateStr to DateValues.
//   ToTimeStr		Convert TimeValues to TimeStr.
//   ToDateStr		Convert DateValues to DateStr.
//   MakeDateTimeStamp	Make a date-timestamp string; e.g., "01-01-2016_12-00-00"
// ----------------------------------------------------------------------------

#include "timetypes.h"
#include <string>
#include <utility>

namespace mtbmpi {

/// Returns a string containing the local date in the format: "yyyy/mm/dd"
std::string DateStr ();

/// Returns a string containing the local time in the format: "hh:mm:ss" using a 24-hour clock.
std::string TimeStr ();

/// Returns a string containing the local date and time in the format
/// "yyyy/mm/dd hh:mm:ss" using a 24-hour clock.
std::string DateTimeStr ();

/// Convert time in string format to TimeValues.
void ToTimeValues (
    TimeValues & timeValues,		///< output time values
    std::string const & timeStr,	///< time in form: HH:MM:SS
    char const separator = ':');	///< separator between time parts

/// Convert date in string format to DateValues.
void ToDateValues (
    DateValues & dateValues,		///< output date values
    std::string const & dateStr,	///< date in form: YYYY/MM/DD
    char const separator = '/');	///< separator between date parts

/// Convert a TimeValues to a string.
std::string & ToTimeStr (
    std::string & timeStr,		///< output time in form: HH:MM:SS
    TimeValues const & timeValues,	///< time values
    char const separator = ':');	///< separator between time parts

/// Convert a TimeValues to a string.
std::string ToTimeStr (
    TimeValues const & timeValues,	///< time values
    char const separator = ':');	///< separator between time parts

/// Convert a DateValues to a string.
std::string & ToDateStr (
    std::string & dateStr,		///< output date in form: YYYY/MM/DD
    DateValues const & dateValues,	///< date values
    char const separator = '/');	///< separator between date parts

/// Convert a DateValues to a string.
std::string ToDateStr (
    DateValues const & dateValues,	///< date values
    char const separator = '/');	///< separator between date parts

/// Make a date-timestamp string of the time provided; e.g., "01-01-2016_12-00-00"
/// @return std::string
std::string MakeDateTimeStamp (
    std::string const & dateStr,	///< date
    std::string const & timeStr,	///< time
    char const separatorValues = '-',	///< separator character between values within a field
    char const separatorFields = '_');	///< separator character between fields

/// Make a date-timestamp string of the time NOW; e.g., "01-01-2016_12-00-00"
/// @return std::string
inline std::string MakeDateTimeStamp (
    char const separatorValues = '-',	///< separator character between values within a field
    char const separatorFields = '_')	///< separator character between fields
{
    return MakeDateTimeStamp( DateStr(), TimeStr(), separatorValues, separatorFields );
}

//---- date-time pair ----

/// Hold a pair of date and time as strings:  1st=date, 2nd=time
typedef std::pair<std::string, std::string>	TDateTime;

/// Make date-time pair from current date and time.
inline TDateTime MakeDateTime ()
{
    return std::make_pair( DateStr(), TimeStr() );
}

/// Make date-time pair from supplied values
inline TDateTime MakeDateTime (
    std::string const & dateStr,		///< date
    std::string const & timeStr)		///< time
{
    return std::make_pair( dateStr, timeStr );
}

/// @cond SKIP_PRIVATE

inline bool operator== (
    TDateTime const & lhs,
    TDateTime const & rhs
    )
{
    return lhs.first == rhs.first &&
	    lhs.second == rhs.second;
}

inline bool operator!= (
    TDateTime const & lhs,
    TDateTime const & rhs
    )
{
    return !(lhs == rhs);
}

/// @endcond


} // namespace mtbmpi


#endif // INC_mtbmpi_timeutil_h
