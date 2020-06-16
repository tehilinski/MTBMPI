/*! ----------------------------------------------------------------------------------------------------------
@file		timetypes.h
@brief 		Types, classes, and functions for common time and date units.
@details
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_timetypes_h
#define INC_mtbmpi_timetypes_h

#include <limits>

namespace mtbmpi {


typedef	long		TYear;			///< year type (signed)
typedef unsigned short	TWeek;			///< week number type (positive)
typedef unsigned short	TDay;			///< day number type (positive)
typedef unsigned short	THour;			///< hour type (positive)
typedef unsigned short	TMinute;		///< minute type (positive)
typedef unsigned short	TSecond;		///< second type (positive)
typedef unsigned long	TLongSecond;		///< long second type (positive)
typedef long		TJulianDay;		///< julian day number type

/// Mimimum possible year value.
static inline TYear MinimumYear () { return std::numeric_limits<TYear>::min(); }

/// Maximum possible year value.
static inline TYear MaximumYear () { return std::numeric_limits<TYear>::max(); }

// ----------------------------------------------------------------------------
/// day of week number
enum TDayOfWeek
{
    // long names
    Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday,
    // short names
    Mon = 1, Tue, Wed, Thu, Fri, Sat, Sun
};

// ----------------------------------------------------------------------------
/// month number type (integer 1-12)
enum TMonth
{
    // long names
    January = 1, February, March, April, June = 6,
    July, August, September, October, November, December,
    // short names
    Jan = 1, Feb, Mar, Apr, May, Jun,
    Jul, Aug, Sep, Oct, Nov, Dec
};

// ----------------------------------------------------------------------------
/// count per interval
enum
{
    daysPerWeek = 7,
    monthsPerYear = 12,
    weeksPerYear = 52,
    hoursPerDay = 24,
    minutesPerHour = 60,
    daysPerYear = 365,
    daysPerLeapYear = 366
};

// ----------------------------------------------------------------------------
/// @class TimeValues
/// @brief Holds a time value as an hour-minute-second triple.
class TimeValues
{
  public:

    THour   hour;		///< hour on a 24-hour clock 0-23
    TMinute minute;		///< minute 0-59
    TSecond second;		///< second 0-59

    TimeValues ()
      {
	Clear ();
      }

    TimeValues (
      THour   const useHour,		///< hour on a 24-hour clock 0-23
      TMinute const useMinute,		///< minute 0-59
      TSecond const useSecond)		///< second 0-59
      : hour   (useHour),
        minute (useMinute),
        second (useSecond)
      {
      }

    TimeValues ( TimeValues const & rhs )
      {
	Copy( rhs );
      }

    TimeValues & operator= ( TimeValues const & rhs )
      {
	Copy( rhs );
	return *this;
      }

    bool operator== ( TimeValues const & rhs ) const
      {
	return
	    hour == rhs.hour &&
	    minute == rhs.minute &&
	    second == rhs.second;
      }

    bool operator!= ( TimeValues const & rhs ) const
      { return !(*this == rhs); }

    bool operator< ( TimeValues const & rhs ) const
      {
	return
	  hour < rhs.hour ?
	    true :
	    ( minute < rhs.minute ?
	      true :
	      second < rhs.second );
      }

    TimeValues operator+ ( TimeValues const & rhs )
      {
	hour   += rhs.hour;
	minute += rhs.minute;
	second += rhs.second;
	if ( second > 59 )
	{
	    minute += static_cast<TMinute>(second / (TSecond)60);
	    second = second % (TSecond)60;
	}
	if ( minute > 59 )
	{
	    hour += static_cast<THour>(minute / (TMinute)60);
	    minute = minute % (TMinute)60;
	}
	// hour can exceed 24 since we don't track days here
	return *this;
      }

    TimeValues operator- ( TimeValues const & rhs )
      {
	unsigned int const secondsRHS = rhs.ToSeconds();
	unsigned int const secondsLSH = ToSeconds();
	unsigned int const secondsDif =
	    ( secondsLSH > secondsRHS ?
	      secondsLSH - secondsRHS :
	      secondsRHS - secondsLSH );
	*this = secondsDif;
	return *this;
      }

    /// Convert time values to integer seconds
    unsigned int ToSeconds () const
      {
	return (unsigned int)second + minute * 60U + hour * 3600U;
      }

    TimeValues & operator= (unsigned int const seconds)
      {
	hour = (THour)( seconds / 3600U );
	TSecond remaining = (TSecond)( seconds % 3600U );
	minute = (TMinute)( remaining / 60U );
	second = remaining % 60;
	return *this;
      }

    void Clear ()
      {
        hour   = 0;
        minute = 0;
        second = 0;
      }

  private:

    /// @cond SKIP_PRIVATE

    void Copy ( TimeValues const & rhs )
      {
	hour   = rhs.hour;
	minute = rhs.minute;
	second = rhs.second;
      }

    /// @endcond
};

// ----------------------------------------------------------------------------
/// @class DateValues
/// @brief calendar date values
class DateValues
{
  public:

    TYear year;			///< year (signed integer)
    TMonth month;		///< month number (1-12)
    TDay day;			///< day number (positive)

    DateValues ()
      {
	Clear ();
      }

    DateValues (
      TYear  const useYear,		///< year (signed integer)
      TMonth const useMonth = Jan,	///< month number (1-12)
      TDay   const useDay = 1)		///< day number (positive)
      : year (useYear),
        month (useMonth),
        day (useDay)
      {
      }

    DateValues ( DateValues const & rhs )
      {
	Copy( rhs );
      }

    DateValues & operator= ( DateValues const & rhs )
      {
	Copy( rhs );
	return *this;
      }

    bool operator== ( DateValues const & rhs ) const
      {
	return
	    year == rhs.year &&
	    month == rhs.month &&
	    day == rhs.day;
      }

    bool operator!= ( DateValues const & rhs ) const
      { return !(*this == rhs); }

    bool operator< ( DateValues const & rhs ) const
      {
	return
	  year < rhs.year ?
	    true :
	    ( month < rhs.month ?
	      true :
	      day < rhs.day );
      }

    void Clear ()
      {
        year = 0;
        month = Jan;
        day = 1;
      }

  private:

    /// @cond SKIP_PRIVATE

    void Copy ( DateValues const & rhs )
      {
	year  = rhs.year;
	month = rhs.month;
	day   = rhs.day;
      }

    /// @endcond
};


} // namespace mtbmpi

// ----------------------------------------------------------------------------
#endif // INC_mtbmpi_timetypes_h
