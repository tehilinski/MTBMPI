/*------------------------------------------------------------------------------------------------------------
file		Communicator.cpp
class		mtbmpi::Communicator
brief 		Manages an MPI::Communicator and Group.
details
		A Communicator allows for a group of specific tasks
		(identified by MPI ranks) to share communication.
		For example, to create a Communicator for the Master/Controller
		and all tasks (excluding Blackboard):

		using mtbmpi::Communicator;
		int const numTasks = MPI::COMM_WORLD.Get_size() - 2;     // all task ranks
		std::vector< int > ranks( numTasks );                    // contains ranks for comm group
		mtbmpi::Sequence< int > sequence ( 2 );                  // sequence starts at rank 2
		std::generate( ranks.begin(), ranks.end(), sequence );   // make a sequence
		ranks.insert( ranks.begin(), 0 );                        // insert rank zero
		std::shared_ptr< Communicator > pComm(
		    new Communicator ( std::string("Task Communicator"), ranks ) );

example		../tests/Test_Communicator.cpp

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

// DEBUGGING
// #define DBG_MPI_COMMUNICATOR

#include "Communicator.h"
#ifdef DBG_MPI_COMMUNICATOR
  #include <iostream>
  using std::cout;
  using std::endl;
#endif


namespace mtbmpi {

    Communicator::Communicator (
	std::string const commName,
	std::vector< int > const & ranks )
	: isInitialized ( false ),
	  group ( MPI::GROUP_NULL ),
	  comm ( MPI::COMM_NULL )
    {
	#ifdef DBG_MPI_COMMUNICATOR
	std::string const myName = "Communicator::Communicator: ";
	cout << myName << "enter" << endl;
	#endif

	if ( MPI::Is_initialized() )
	    Initialize( commName, ranks );

	#ifdef DBG_MPI_COMMUNICATOR
	cout << myName << "done" << endl;
	#endif
    }

    /// @cond SKIP_PRIVATE

    void Communicator::Initialize (
	std::string const & commName,			// name of MPI communicator
	std::vector < int > const & ranks )		// ranks of task to include
    {
	#ifdef DBG_MPI_COMMUNICATOR
	std::string const myName = "Communicator::Initialize: ";
	cout << myName << "enter" << endl;
	#endif

	#ifdef DBG_MPI_COMMUNICATOR
	cout << myName << "making group" << endl;
	#endif
	group = MPI::COMM_WORLD.Get_group().Incl( ranks.size(), &ranks[0] );
	if ( group ==  MPI::GROUP_NULL ||
	     (int) group.Get_size() != (int) ranks.size() )
	{
	    throw std::runtime_error( "Communicator: Could not create MPI group." );
	}
	#ifdef DBG_MPI_COMMUNICATOR
	cout << myName << "group size = " << group.Get_size() << endl;
	#endif

	// make new communicator for this group
	// The caller's ranks will be renumbered sequentially and w/out gaps
	#ifdef DBG_MPI_COMMUNICATOR
	cout << myName << "making communicator" << endl;
	#endif
	comm = MPI::COMM_WORLD.Create( group );
	if ( comm != MPI::COMM_NULL && comm.Get_rank() == group.Get_rank() )
	{
	    comm.Set_errhandler( MPI::ERRORS_THROW_EXCEPTIONS );
	    comm.Set_name( commName.c_str() );
	    isInitialized = true;
	}

	#ifdef DBG_MPI_COMMUNICATOR
	cout << myName << "done" << endl;
	#endif
    }

    void Communicator::Close ()
    {
	if ( isInitialized )
	{
	    comm.Free();
	    isInitialized = false;
	}
	comm = MPI::COMM_NULL;
	group = MPI::GROUP_NULL;
    }

    Communicator::~Communicator ()
    {
	Close();
    }

    /// @endcond

} // namespace mtbmpi
