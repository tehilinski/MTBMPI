/*! ----------------------------------------------------------------------------------------------------------
@file		ErrorHandling.h
@class		mtbmpi::ErrorHandling
@brief 		Provides error handling functions that are (hopefully) consistent
		across MPI implementations.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_ErrorHandling_h
#define INC_mtbmpi_ErrorHandling_h

#include "mpi.h"
#include <iosfwd>
#include <sstream>

namespace mtbmpi {


// known preproccessor macros:
//   MPICH
//   OPEN_MPI


///	Check for unhandled error in MPI. Throw exception if found.
///	If OpenMPI is compiled with C++ exception handling turned off, then hopefully
///	this will find an error.
///	@param origin	Location of the error
void CheckErrorMPI ( std::string const origin = std::string() );

/// Get name of the MPI error handler.
inline std::string GetErrorHandlerName (MPI::Errhandler const & errHandler )
{
    if ( errHandler == MPI::ERRORS_THROW_EXCEPTIONS )
	return "ERRORS_THROW_EXCEPTIONS";
    else if ( errHandler == MPI::ERRORS_RETURN )
	return "ERRORS_RETURN";
    else if ( errHandler == MPI::ERRORS_ARE_FATAL )
	return "ERRORS_ARE_FATAL";
    else
	return "Unknown error handler";
}

/// Set the MPI error handler for the copmmunicator.
std::string SetErrorHandler (
    MPI::Intracomm & comm,
    MPI::Errhandler const & errorHandler = MPI::ERRORS_THROW_EXCEPTIONS );

/// Retrieves an implementation-dependent error string.
std::string GetErrorString ( MPI::Exception & e );

/// Get the error string associated with the error code.
std::string GetErrorString ( int const errorCode );


#if defined(OPEN_MPI) && !defined(OMPI_HAVE_CXX_EXCEPTION_SUPPORT)
    inline std::string GetErrorString ()
    {
	return GetErrorString( MPI::mpi_errno );
    }
#endif


} // namespace mtbmpi


#endif // INC_mtbmpi_ErrorHandling_h
