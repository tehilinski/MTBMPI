/*------------------------------------------------------------------------------------------------------------
file		ErrorHandling.cpp
class		mtbmpi::ErrorHandling
brief 		Provides error handling functions that are (hopefully) consistent
		across MPI implementations.

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "ErrorHandling.h"

namespace mtbmpi {


// Check for unhandled error in MPI. Throw exception if found.
// If OpenMPI is compiled with C++ exception handling turned off, then hopefully
// this will find an error.
void CheckErrorMPI ( std::string const origin )
{
    #if defined(OPEN_MPI) && !defined(OMPI_HAVE_CXX_EXCEPTION_SUPPORT)
	if ( MPI::mpi_errno != MPI_SUCCESS )
	{
	    std::string msg = GetErrorString();
	    if ( !msg.empty() )
		msg = std::string("MPI error: ") + msg;
	    else
		msg = "Unknown MPI error";
	    if ( !origin.empty() )
		msg = msg + std::string(": origin = ") + origin;
	    throw std::runtime_error( msg );
	}
    #else
    #endif
}

std::string SetErrorHandler (
    MPI::Intracomm & comm,
    MPI::Errhandler const & errorHandler )
{
    std::string msg;
    comm.Set_errhandler( errorHandler );
    if ( comm.Get_errhandler() != MPI::ERRORS_THROW_EXCEPTIONS )
    {
	msg = "MPI Error handler does not throw exceptions; handler is ";
	msg += GetErrorHandlerName( comm.Get_errhandler() );
    }
    return msg;
}

std::string GetErrorString ( MPI::Exception & e )
{
    std::ostringstream oss;
    oss << "MPI Error: ";
    if ( MPI::Is_initialized() )
	oss << "process " << MPI::COMM_WORLD.Get_rank();
    oss << "\nMPI error: " << e.Get_error_string();
    oss << "\nerror class = " << e.Get_error_class()
	<< ": error code = "  << e.Get_error_code();
    return oss.str();
}

std::string GetErrorString ( int const errorCode )
{
    int length = 0;
    std::string msg;
    msg.resize( MPI_MAX_ERROR_STRING );
    MPI::Get_error_string ( errorCode, &msg[0], length );
    return msg;
}


} // namespace mtbmpi
