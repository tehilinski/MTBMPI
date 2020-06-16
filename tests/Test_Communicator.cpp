//------------------------------------------------------------------------------------------------------------
// File: Test_Communicator.cpp
// Test of class mtbmpi::Communicator.
// Build:
//	mpicxx -I../src -o Test_Communicator -g Test_Communicator.cpp ../build/cmake/libmtbmpi.debug.a
// Run:
//	mpiexec -n 3 ./Test_Communicator | sort
//
// project	Master-Task-Blackboard MPI Framework
// author	Thomas E. Hilinski <https://github.com/tehilinski>
// copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
// 		This software library, including source code and documentation,
// 		is licensed under the Apache License version 2.0.
// 		See "LICENSE.md" for more information.
//------------------------------------------------------------------------------------------------------------

#include <iostream>
using std::cout;
using std::endl;
#include <exception>
#include <memory>
#include <algorithm>

#include "Communicator.h"
#include "UtilitiesMPI.h"
#include "ErrorHandling.h"

void DisplayMsg ( int const rank,
		  std::string const part1,
		  std::string const part2 = std::string(),
		  std::string const part3 = std::string(),
		  std::string const part4 = std::string() )
{
    static short msgNum = 0;
    ++msgNum;
    cout << "rank " << rank << "  " << msgNum << ": " << part1;
    if ( !part2.empty() )
	cout << " " << part2;
    if ( !part3.empty() )
	cout << " " << part3;
    if ( !part4.empty() )
	cout << " " << part4;
    cout << endl;
}

//------------------------------------------------------------------------------------------------------------

inline int GetNumberOfProcesses() { return MPI::COMM_WORLD.Get_size(); }

std::vector<int> MakeRanksSequence ( int const numTasks )
{
    std::vector<int> ranks( numTasks );				// contains ranks for comm group
    mtbmpi::Sequence< int > sequence ( 2 );			// sequence starts at rank 2
    std::generate( ranks.begin(), ranks.end(), sequence );	// make a sequence
    ranks.insert( ranks.begin(), 0 );				// insert rank zero
    return ranks;
}

void DisplayRanks ( std::vector<int> ranks )
{
    cout << "Communicator ranks =";
    for ( int i = 0; i < ranks.size(); ++i )
	cout << " " << ranks[i];
    cout << endl;
}

//------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    int myRank = -1;
    try
    {
	MPI::Init( argc, argv );
	MPI::COMM_WORLD.Set_errhandler( MPI::ERRORS_THROW_EXCEPTIONS );

	myRank = MPI::COMM_WORLD.Get_rank();
	if ( myRank == 0 )
		cout << "\nTest of class mtbmpi::Communicator" << endl;
	DisplayMsg( myRank, "MPI initialized" );

	// ranks
	int const numTasks = GetNumberOfProcesses() - 2; 	// all task ranks
	std::vector< int > ranks = MakeRanksSequence( numTasks );	// includes rank zero
	if ( myRank == 0 )
	    DisplayRanks( ranks );

	// Communicator
	std::shared_ptr< mtbmpi::Communicator > pComm (
	    new mtbmpi::Communicator ( std::string("Task Communicator"), ranks ) );
	if ( pComm->IsInitialized() )
	    DisplayMsg( myRank, "Communicator created" );
	pComm.reset();

	DisplayMsg( myRank, "stopping" );
	DisplayMsg( myRank, mtbmpi::GetErrorString( MPI::mpi_errno ) );
	if ( MPI::Is_initialized() )
	    MPI::Finalize();
    }
    catch ( MPI::Exception & e )
    {
	DisplayMsg( myRank, mtbmpi::GetErrorString(e) );
	MPI::COMM_WORLD.Abort( e.Get_error_code() );
    }
    catch (std::exception const & e)
    {
	cout << "Error: " << e.what() << endl;
    }

    DisplayMsg( myRank, "   all done!" );
    return 0;
}
