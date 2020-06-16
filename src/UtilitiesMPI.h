/*! ----------------------------------------------------------------------------------------------------------
@file		UtilitiesMPI.h
@brief 		Handy functions for working with MPI and the MTBMPI library.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Utilities_h
#define INC_mtbmpi_Utilities_h

#include "timeutil.h"
#include <string>
#include <vector>
#include <sstream>

// Using Microsoft Windows?
#if defined(__CYGWIN__)
  #ifndef CYGWIN
    #define CYGWIN 1
  #endif
#endif
#if (defined(_Windows) || defined(__WIN32__) || defined(_WIN32) || \
     defined(_WIN64) || defined(_Wp64) || \
     defined(_MSC_VER) || defined(_MFC_VER) || defined(__BORLANDC__))
  #ifndef MSWINDOWS
    #define MSWINDOWS 1
  #endif
#endif


namespace mtbmpi {


typedef std::vector< std::string >	StrVec;


extern std::string const EMPTY_STRING_STATIC;

static char const NULL_CHAR	= '\0';
static char const BLANK_CHAR	= ' ';
static char const NL_CHAR	= '\n';


///	Get MPI processor name.
std::string GetMPIProcessorName ();

///	Cause this process to sleep.
///	@param usec	Sleep duration (microseconds); default is 1000.
void Sleep ( unsigned int const usec = 1000 );

///	Transfers char** to a StrVec.
///	@param strArray		vector of strings to receive the C strings
///	@param beginCStr	pointer to start of C strings
///	@param endCStr		pointer to last C string + 1
///	@return			number of C strings transferred (not size of StrVec).
StrVec::size_type ToStrVec (
	StrVec & strArray,
	char const * const * const beginCStr,
	char const * const * const endCStr);

///	Parse string into char-delimited tokens.
///	@param line		line to parse
///	@param tokenArray	vector of strings to receive the tokens
///	@param delimiter	character delimiting tokens; default char is a blank
///	@param maxNumber	maximum number of tokens to parse; if zero, parse all
///	@return		size of of vector.
StrVec::size_type ParseTokens (
	std::string const & line,
	StrVec & tokenArray,
	char const delimiter = BLANK_CHAR,
	short const maxNumber = 0 );

///	Joins all strings in a vector, separated by char.
///	@param str		string holds results
///	@param strArray		vector of strings to join
///	@param separator	char to separate strings
///	@return			length of the concatenated string.
std::string::size_type JoinStrings (
    std::string & str,
    StrVec const & strArray,
    char const separator = BLANK_CHAR);

/// Generates a sequence of integers with a stride.
template< typename T >		///< integer type
class Sequence
{
  public:

    Sequence ( T const start = 0, T const increment = 1 )
      : value  ( start ),
        stride ( increment )
      {
      }

    T operator() ()
      {
	T const curValue = value;
	value += stride;
	return curValue;
      }

  private:

    T value;
    T const stride;
};

/// Make a date+timestamp prefix for a message display.
static inline std::string DateTimeStampPrefix ()
{
    std::string prefix = MakeDateTimeStamp();
    prefix += ": ";
    return prefix;
}

// -----------------------------------------------------------------------------------------------------------
//	Converts any type to a string.
//	Source: John Torjo, "to_string"
// -----------------------------------------------------------------------------------------------------------

template < class T >
inline std::string ToString( T const &value )
{
    std::ostringstream streamOut;
    streamOut << value;
    return streamOut.str(  );
}

template <>
inline std::string ToString < std::string > ( std::string const &s )
{
    return s;
}

template <>
inline std::string ToString < char const *>(
    char const *const &cStr )
{
    return cStr;
}

// -----------------------------------------------------------------------------------------------------------
//	Extracts any type from a string.
//	This works for any built-in type and any class with a
//	default constructor that has implemented the >> operator.
//	Source: John Torjo, "from_string"
// -----------------------------------------------------------------------------------------------------------

template < class T >
inline T FromString( std::string const &stringIn )
{
    std::istringstream streamIn( stringIn );
    T returnValue = T(  );
    streamIn >> returnValue;
    return returnValue;
}

template <>
inline std::string FromString < std::string > ( std::string const &stringIn )
{
    return stringIn;
}

template < class T >
inline void FromString(
    StrVec const &sa,
    std::vector < T > &fsa )
{
    fsa.resize( sa.size(  ) );
    typename std::vector < T >::iterator iFSA = fsa.begin(  );
    for ( StrVec::const_iterator iSA = sa.begin(  ); iSA != sa.end(  ); ++iSA, ++iFSA )
	*iFSA = FromString < T > ( *iSA );
}


} // namespace mtbmpi


std::ostream & operator<< (
    std::ostream & os,
    mtbmpi::StrVec const & v);


#endif // INC_mtbmpi_Utilities_h
