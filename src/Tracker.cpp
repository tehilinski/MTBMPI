/*------------------------------------------------------------------------------------------------------------
file		Tracker.cpp
class		mtbmpi::Tracker
brief 		Tracks the state of each tasks process, not including the
		Master, Controller, and Blackboard.
details		Task IDs are zero-based, and include only work tasks.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "Tracker.h"
#include <sstream>
#include <stdexcept>

namespace mtbmpi {


Tracker::Tracker (
    int const numTasks)		// number of work processes
{
    taskStateArray.assign (numTasks, State_Unknown);
}

State Tracker::SetState (		// set task state
    size_type const index,		//   at zero-based index
    State const newState)		//   to this state
{
    if ( index < taskStateArray.size() )
    {
	State const oldState = taskStateArray[index];
	taskStateArray[index] = newState;
	return oldState;
    }
    else // ERROR: should never get here
    {
	std::ostringstream os;
	os << "mtbmpi::Tracker: invalid index to state array: "
	   << index;
	throw std::runtime_error( os.str() );
    }
}

bool Tracker::AreAllCreated () const	// true if all tasks created
{
    bool created = true;
    StateArray::const_iterator i = taskStateArray.begin();
    while ( i != taskStateArray.end() && created )
    {
	created &= IsCreated( *i );
	++i;
    }
    return created;
}

bool Tracker::AreAllInitialized () const	// true if all tasks initialized
{
    bool allInit = true;
    StateArray::const_iterator i = taskStateArray.begin();
    while ( i != taskStateArray.end() && allInit )
    {
	allInit &= IsInitialized( *i );
	++i;
    }
    return allInit;
}

bool Tracker::AreAllStopped () const	// true if all tasks stopped
{
    bool stopped = true;
    StateArray::const_iterator i = taskStateArray.begin();
    while ( i != taskStateArray.end() && stopped )
    {
	stopped &= ( IsCompleted( *i ) ||
		     IsTerminated( *i ) ||
		     IsError( *i ) ||
		     IsUnknown( *i ) );
	++i;
    }
    return stopped;
}


} // namespace mtbmpi
