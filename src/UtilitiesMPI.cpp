/*------------------------------------------------------------------------------------------------------------
file		UtilitiesMPI.cpp
brief 		Handy functions for working with MPI and the MTBMPI library.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "UtilitiesMPI.h"
#include "ErrorHandling.h"
#include <cstring>
#ifdef MSWINDOWS
  #include <winbase.h>
#else
  #include <unistd.h>
#endif


namespace mtbmpi {


std::string const EMPTY_STRING_STATIC;


std::string GetMPIProcessorName ()
{
    std::string nameStr;
    char name[MPI_MAX_PROCESSOR_NAME + 1];
    std::memset( name, 0xFF, MPI_MAX_PROCESSOR_NAME + 1 );
    int actualLength = 0;
    MPI::Get_processor_name( name, actualLength );
    if (actualLength <= MPI_MAX_PROCESSOR_NAME && actualLength > 0)
	nameStr = std::string(name).substr( 0, actualLength );
    return nameStr;
}

void Sleep ( unsigned int const usec )
{
    #ifdef MSWINDOWS
      // type: DWORD dwMilliseconds
      ::Sleep( usec == 0 ? 1 : (usec <= 1000 ? 1 : usec/1000) );
    #else
      // type: unsigned int microseconds
      ::usleep( usec == 0 ? 1000 : usec );
    #endif
}

StrVec::size_type ToStrVec (
	StrVec & strArray,
	char const * const * const beginCStr,
	char const * const * const endCStr)
{
	StrVec::size_type count = 0;
	char const * const * i = beginCStr;
	while ( i != endCStr )
	{
		if ( i != 0 && (*i) != 0 )
		{
			strArray.push_back (*i);
			++count;
		}
		++i;
	}
	return count;
}

StrVec::size_type ParseTokens (
    std::string const & line,
    StrVec & tokenArray,
    char const delimiter,
    short const maxNumber)
{
    short maxTokens = maxNumber > 0 ? maxNumber : line.length();
    short countParsed = 0;
    StrVec::size_type const initialSize = tokenArray.size();
    std::istringstream iss ( line );
    while ( iss.good() && countParsed < maxTokens )
    {
	if ( iss )
	    iss >> std::ws;
	if ( iss )
	{
	    std::string token;
	    std::getline (iss, token, delimiter);
	    tokenArray.push_back (token);
	    ++countParsed;
	}
    }
    return tokenArray.size() - initialSize;
}

std::string::size_type JoinStrings (
    std::string & str,		// string holds results
    StrVec const & strArray,	// array of strings to join
    char const separator)		// char separates strings
{
    StrVec::const_iterator i = strArray.begin();
    while ( i != strArray.end() )
    {
	if ( !str.empty() && separator != NULL_CHAR )
	    str += separator;
	str += *(i++);
    }
    return str.size();
}


} // namespace mtbmpi


std::ostream & operator<< (
    std::ostream & os,
    mtbmpi::StrVec const & v)
{
    // do output
    char const NL = '\n';
    os << "[ ";
    for ( mtbmpi::StrVec::const_iterator iV = v.begin(); iV != v.end(); )
    {
	os << (*iV);
	++iV;
	if ( iV != v.end() )
	    os << NL << "  ";
    }
    os << " ]";
    os.flush();
    return os;
}
