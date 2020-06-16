/*! ----------------------------------------------------------------------------------------------------------
@file		Tracker.h
@class		mtbmpi::Tracker
@brief 		Tracks the state of each tasks process, not including the
		Master, Controller, and Blackboard.
@details	Task IDs are zero-based, and include only work tasks.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Tracker_h
#define INC_mtbmpi_Tracker_h

#include "State.h"
#include "MsgTags.h"
#include <vector>

namespace mtbmpi {


class Tracker
{
  public:

    typedef std::vector<State>			StateArray;	///< Array of states, one per task
    typedef StateArray::size_type		size_type;	///< length of StateArray
    typedef StateArray::const_iterator		const_iterator;

    /// Constructor
    Tracker (
      int const numTasks			///< number of work processes (tasks)
      );	// here for doxygen bug

    /// set task state
    State SetState (
      size_type const index,			///< task index in state array is zero-based
      State const newState			///< new task this state
      ); // this is here because of doxygen bug

    /// get task state
    State GetState (
      size_type const index			///< task index in state array is zero-based
      ) const // this is here because of doxygen bug
      { return taskStateArray[index]; }

    /// number of tasks tracked
    size_type Size () const { return taskStateArray.size(); }		///< number of tasks begin tracked

    const_iterator Begin () const { return taskStateArray.begin(); }	///< starting iterator of States
    const_iterator End ()   const { return taskStateArray.end(); }	///< end iterator of States

    bool AreAllCreated () const;		///< true if all tasks created
    bool AreAllInitialized () const;		///< true if all tasks initialized
    bool AreAllStopped () const;		///< true if all tasks stopped

  private:

    /// @cond SKIP_PRIVATE

    StateArray taskStateArray;			// task states

    // functions that should not be used; are not defined
    Tracker (Tracker const & object);
    Tracker & operator= (Tracker const & object);
    bool operator== (Tracker const & object) const;
    bool operator!= (Tracker const & object) const;

    /// @endcond
};


} // namespace mtbmpi

#endif // INC_mtbmpi_Tracker_h
