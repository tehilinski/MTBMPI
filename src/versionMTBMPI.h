/*! ----------------------------------------------------------------------------------------------------------
@file		versionMTBMPI.h
@class		mtbmpi::versionMTBMPI
@brief 		Master-Task-Blackboard MPI Framework version information.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_versionMTBMPI_h
#define INC_mtbmpi_versionMTBMPI_h

// ----------------------------------------------------------------------------
//	Macro:			Description:
//	DEBUG_STATE_MTBMPI	Defined if in debug build.
// ----------------------------------------------------------------------------

#include "VersionData.h"

#if !defined(DEBUG_STATE_MTBMPI)
  #if defined(_DEBUG) || !defined(NDEBUG)
    #define DEBUG_STATE_MTBMPI
  #endif
#endif

namespace mtbmpi {

    /// MTBMPI framework version information
    extern VersionData const versionMTBMPI;

} // namespace mtbmpi

#endif // INC_mtbmpi_versionMTBMPI_h
