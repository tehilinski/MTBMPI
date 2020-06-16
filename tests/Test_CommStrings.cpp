//------------------------------------------------------------------------------------------------------------
// File: Test_CommStrings.cpp
// Test of class mtbmpi::CommStrings.
// Build:
//	mpicxx -DDEBUG_CommStrings -I../src -o Test_CommStrings -g Test_CommStrings.cpp ../build/cmake/libmtbmpi.debug.a
// Run:
//	mpiexec -n 3 ./Test_CommStrings | sort
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
#include <sstream>
#include <exception>
#include <algorithm>
#include <memory>
#include "MTBMPI.h"
#include "UtilitiesMPI.h"

char const * const appTitle = "Test of class mtbmpi::CommStrings";


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

std::string MakeRanksString ( std::vector<int> const & ranks )
{
    std::ostringstream oss;
    for ( int i = 0; i < ranks.size(); ++i )
	oss << " " << ranks[i];
    oss << std::ends;
    return oss.str();
}

//------------------------------------------------------------------------------------------------------------

void StopBlackboard ()
{
    // send request stop
    mtbmpi::comm.Send ( 0, 0, MPI::BYTE, 1, mtbmpi::Tag_StopBlackboard );
    // wait for confirmation
    mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, 1, mtbmpi::Tag_Confirmation );
    // time for BB to actually stop
    mtbmpi::Sleep();
}

//------------------------------------------------------------------------------------------------------------

std::vector<int> MakeRanksSequence ( int const numTasks )
{
    std::vector<int> ranks( numTasks );				// contains ranks for comm group
    mtbmpi::Sequence< int > sequence ( 2 );			// sequence starts at rank 2
    std::generate( ranks.begin(), ranks.end(), sequence );	// make a sequence
    ranks.insert( ranks.begin(), 0 );				// insert rank zero
    return ranks;
}

void SendStrings (
    mtbmpi::Communicator & comm,
    std::vector< int > const & destRanks )	// destination ranks are in MPI::COMM_WORLD, not in comm
{
    int const myRank = comm.GetGroup().Get_rank();
    mtbmpi::StrVec strVec = { "line 1", "line 2", "last line" };
    mtbmpi::LoggerMPI logger ( 1 );

    std::ostringstream oss;
    oss << "Sending strings:\n" << strVec
	<< "\nTo ranks = " << MakeRanksString( destRanks )
	<< std::ends;
    logger.Message( oss.str() );

    mtbmpi::CommStrings commStrings ( myRank, logger, comm );
    for ( int taskID = 1; taskID < comm.GetComm().Get_size(); ++taskID )
    {
	commStrings.Isend( taskID, mtbmpi::Tag_Data, strVec );
	std::ostringstream oss;
	oss << "Sent msg to rank " << taskID
	    << ": " << mtbmpi::GetErrorString( MPI::mpi_errno )
	    << std::ends;
	DisplayMsg( myRank, oss.str() );
    }
    DisplayMsg( myRank, "SendStrings finished sending." );
    commStrings.WaitAll();
    mtbmpi::Sleep();	// let msgs finish receive
}

void ReceiveStrings (
    mtbmpi::Communicator & comm )
{
    int const myRank = comm.GetGroup().Get_rank();
    mtbmpi::LoggerMPI logger ( 1 );

    DisplayMsg( myRank, "ReceiveStrings begin." );
    mtbmpi::StrVec strVec;

    mtbmpi::CommStrings commStrings ( myRank, logger, comm );
    commStrings.Receive ( 0, mtbmpi::Tag_Data,  strVec );
    std::ostringstream oss;
    oss << "Received strings:\n" << strVec << std::ends;
    logger.Message( oss.str() );
}

//------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    int myRank = -1;
    try
    {
	MPI::Init( argc, argv );
	MPI::COMM_WORLD.Set_errhandler( MPI::ERRORS_THROW_EXCEPTIONS );
	mtbmpi::comm = MPI::COMM_WORLD.Dup();

	myRank = MPI::COMM_WORLD.Get_rank();
	if ( myRank == 0 )
	    cout << appTitle << endl;
	DisplayMsg( myRank, "MPI initialized" );

	// communicator with master + tasks
	int const numTasks = MPI::COMM_WORLD.Get_size() - 2; 		// all tasks; exclude Blackboard
	std::vector< int > ranks = MakeRanksSequence( numTasks );	// task ranks + rank zero
	mtbmpi::Communicator comm ( "Tasks", ranks );
	DisplayMsg( myRank, "Communicator initialized from ranks = ", MakeRanksString(ranks) );

	/// test CommStrings
	if ( myRank == 0 )			// Master
	{
	    SendStrings( comm, ranks );
	}
	else if ( myRank == 1 )			// Blackboard
	{
	    std::string logFileName = "Test_CommStrings.log";
	    DisplayMsg( myRank, "log file: ", logFileName );
	    mtbmpi::Blackboard bb ( myRank, 0, std::make_shared<mtbmpi::OutputMgr_NoOp>(), logFileName );
	    bb.GetRunLogMgr().Write( appTitle );
	    bb.Activate();
	}
	else if ( myRank > 1 )			// tasks
	{
	    ReceiveStrings( comm );
	}

	if ( myRank == 0 )
	    StopBlackboard();
	comm.Close();

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
