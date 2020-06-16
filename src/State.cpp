/*------------------------------------------------------------------------------------------------------------
file		State.cpp
brief 		Provides enum and helper functions for the state of a process.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "State.h"

namespace mtbmpi {


std::string AsString (State const state)
{
    switch (state)
    {
	case State_Created:     return std::string( "Created" );
	case State_Initialized: return std::string( "Initialized" );
	case State_Running:     return std::string( "Running"     );
	case State_Completed:   return std::string( "Completed"   );
	case State_Paused:      return std::string( "Paused"      );
	case State_Terminated:  return std::string( "Terminated"  );
	case State_Error:       return std::string( "Error"       );
	default:
	case State_Unknown:     return std::string( "Unknown"     );
    };
}


} // namespace mtbmpi
