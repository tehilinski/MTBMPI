/*! ----------------------------------------------------------------------------------------------------------
@file		Communicator.h
@class		mtbmpi::Communicator
@brief 		Manages an MPI::Communicator and Group.
@details
		A Communicator allows for a group of specific tasks
		(identified by MPI ranks) to share communication.
		For example, to create a Communicator for the Master/Controller
		and all tasks (excluding Blackboard):
@code
		using mtbmpi::Communicator;
		int const numTasks = MPI::COMM_WORLD.Get_size() - 2;     // all task ranks
		std::vector< int > ranks( numTasks );                    // contains ranks for comm group
		mtbmpi::Sequence< int > sequence ( 2 );                  // sequence starts at rank 2
		std::generate( ranks.begin(), ranks.end(), sequence );   // make a sequence
		ranks.insert( ranks.begin(), 0 );                        // insert rank zero
		std::shared_ptr< Communicator > pComm(
		    new Communicator ( std::string("Task Communicator"), ranks ) );
@endcode
@example	../tests/Test_Communicator.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Communicator_h
#define INC_mtbmpi_Communicator_h

#include "mpi.h"
#include <string>
#include <vector>

namespace mtbmpi
{

    class Communicator
    {
	public:

	    /// Constructor
	    Communicator (
		std::string const commName,			///< name of MPI communicator
		std::vector< int > const & ranks		///< ranks of task to include
		);	// here for doxygen bug

	    bool IsInitialized () const { return isInitialized; }

	    MPI::Group & GetGroup () { return group; }

	    MPI::Intracomm & GetComm () { return comm; }

	    void Close ();

	    ~Communicator ();

	private:

	    /// @cond SKIP_PRIVATE

	    bool isInitialized;		// true if group and comm are initialized
	    MPI::Group group;		// MPI Collector group
	    MPI::Intracomm comm;	// MPI communicator for collecting output

	    void Initialize (
		std::string const & commName,			// name of MPI communicator
		std::vector < int > const & ranks );		// ranks of task to include


	    // unused functions
	    Communicator (Communicator const & rhs);
	    Communicator & operator= (Communicator const & rhs);
	    bool operator== (Communicator const &rhs) const;
	    bool operator!= (Communicator const & rhs) const;

	    /// @endcond
    };

} // namespace mtbmpi


#endif // INC_mtbmpi_Communicator_h
