/*! ----------------------------------------------------------------------------------------------------------
@file		State.h
@brief 		Provides enum and helper functions for the state of a process.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_State_h
#define INC_mtbmpi_State_h

#include <string>

namespace mtbmpi {

    /// Tags which label the state of a process.
    enum State
    {
	State_Created = 0,	///< task has been instantiated
	State_Initialized,	///< task has been initialized
	State_Running,		///< task is currently running
	State_Completed,	///< task has completed without an error
	State_Paused,		///< task has paused
	State_Terminated,	///< task has been terminated without an error
	State_Error,		///< task has stopped with an error
	State_Unknown = 9	///< unknown state = initial state of a task
    };

    inline bool IsCreated (State const s)     { return s == State_Created; }    	///< instantiated?
    inline bool IsInitialized (State const s) { return s == State_Initialized; }	///< initialized?
    inline bool IsRunning (State const s)     { return s == State_Running; }    	///< running?
    inline bool IsCompleted (State const s)   { return s == State_Completed; }  	///< completed?
    inline bool IsPaused (State const s)      { return s == State_Paused; }     	///< paused?
    inline bool IsTerminated (State const s)  { return s == State_Terminated; } 	///< terminated?
    inline bool IsError (State const s)       { return s == State_Error; }      	///< error stop?
    inline bool IsUnknown (State const s)     { return s == State_Unknown; }    	///< unknown state?

    std::string AsString (State const state);		///< Return a string description of the state

} // namespace mtbmpi


#endif // INC_mtbmpi_State_h
