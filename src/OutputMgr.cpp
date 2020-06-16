/*------------------------------------------------------------------------------------------------------------
file		OutputMgr.cpp
class		mtbmpi::OutputMgr
brief 		Manages the output from simulations.
details
		This is owned by the Blackboard.
		Your application can inherit OutputMgr to provide
		the actual output functionality.

example		../examples/OutputMgrExample.cpp

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "OutputMgr.h"
#include "Blackboard.h"
#include "UtilitiesMPI.h"

namespace mtbmpi {


OutputMgr::OutputMgr (
    OutputFactoryPtr useOutputFactory )
    : pOutputFactory ( useOutputFactory )
{
    if ( pOutputFactory.get() != nullptr )
	pOutputAdapter = pOutputFactory->Create( *this );
    else			// should never get here
	throw std::runtime_error( "OutputMgr: output factory pointer is null." );
}

OutputMgr::~OutputMgr ()
{
}

void OutputMgr::HandleOutputMessage (
    MPI::Intracomm & comm,		// MPI communicator with message
    MPI::Status const & status )	// Status from probe contains source ID and message tag
{
    // do a recv so message is marked as received
    char charBuffer = NULL_CHAR;
    mtbmpi::comm.Recv ( &charBuffer, 1, MPI::BYTE, status.Get_source(), status.Get_tag() );
}


} // namespace mtbmpi
