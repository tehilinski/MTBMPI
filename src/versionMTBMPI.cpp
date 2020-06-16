/*------------------------------------------------------------------------------------------------------------
file		versionMTBMPI.cpp
class		mtbmpi::versionMTBMPI
brief 		Master-Task-Blackboard MPI Framework version information.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
//	Macros that can be used externally:
//	DEBUG_STATE_MTBMPI		Defined if in debug build.
// ----------------------------------------------------------------------------

#include "versionMTBMPI.h"

namespace mtbmpi {

    VersionData const versionMTBMPI (
	"MTBMPI",						// product's short name
	"Master-Task-Blackboard MPI Framework",			// product's long name
	"2.0.0",						// version number string
								// Format: n.n.n where each number is
								// "major"."minor"."release"
	"Master-Task-Blackboard MPI framework for models.",	// product description
	"Copyright 2020 Thomas E. Hilinski. "			// product copyright
	"All rights reserved.",
	"",							// company/organization
	"https://github.com/tehilinski",			// product web address
	#if !defined(DEBUG_STATE_MTBMPI)			// special build flags
	  "",
	#else
	  "DEBUG (" __DATE__ ")",
	#endif
	"",							// product license name
	"");							// license web address

} // namespace mtbmpi
