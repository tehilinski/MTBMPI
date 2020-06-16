/*! ----------------------------------------------------------------------------------------------------------
@file		Configuration.h
@class		mtbmpi::Configuration
@brief 		Holds the configuration data for the application.
@details
		Converts the raw command-line arguments to a vector<string>.

		The tasks and master have access to the same configuration object.

		This class can be inherited and extended to provide additional processing
		of command-line arguments.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Configuration_h
#define INC_mtbmpi_Configuration_h

#include "UtilitiesMPI.h"

namespace mtbmpi {

    class Configuration
    {
      public:

	/// Constructor
	Configuration (
	    int const argc,				///< number of command-line arguments
	    char const * const * const argv)		///< raw command-line arguments
	{
	    ToStrVec (args, argv + 1, argv + argc);
	}

	StrVec const & GetArgs () const { return args; }

	virtual ~Configuration () {}

      private:

	/// @cond SKIP_PRIVATE

	StrVec args;

	// functions that should not be used; are not defined
	Configuration (Configuration const & object);
	Configuration & operator= (Configuration const & object);
	bool operator== (Configuration const & object) const;
	bool operator!= (Configuration const & object) const;

	/// @endcond
    };

} // namespace mtbmpi


#endif // INC_mtbmpi_Configuration_h
