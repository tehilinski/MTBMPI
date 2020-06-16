/*! ----------------------------------------------------------------------------------------------------------
@file		CommStrings.h
@class		mtbmpi::CommStrings
@brief 		Sends an array of strings to specified tasks, using the specified MPI communicator.
@example	../tests/Test_CommStrings.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_CommStrings_h
#define INC_mtbmpi_CommStrings_h

#include "mpi.h"
#include "Communicator.h"
#include "SendsMsgsToLog.h"
#include "MsgTags.h"
#include "UtilitiesMPI.h"

namespace mtbmpi {

    class CommStrings
    {
      public:

	typedef TaskID::IDNum		IDNum;

	/// Constructor
	CommStrings (
	  int const parentID,			///< sends rank
	  LoggerMPI & logger,			///< receives log messages
	  Communicator & useComm		///< communicator to send messages
	  );	// here for doxygen bug

	~CommStrings ();

	/// Call by sender only; sends the string array to the destination process
	void Isend (
	  IDNum const destinationID,		///< destination task ID
	  MsgTags const msgTag,			///< tag ID for message
	  StrVec & strVec );			///< strings to send

	/// Call by receiver only; receives the string array
	void Receive(
	  IDNum const sourceID,			///< source task ID
	  MsgTags const msgTag,			///< tag ID for message
	  StrVec & strVec );			///< returns received strings

	/// Call by sender only; call after all Sends are done
	void WaitAll ();

	/// number of sends == number of destination processes
	int GetSendCount () const { return sendCount; }

	static char const lineDelimiter;		///< packed data delimiter

      private:

	/// @cond SKIP_PRIVATE

	typedef std::vector<char>		TBufferData;	// bytes
	typedef std::pair< TBufferData, int>	TBuffer;	// data, size
	typedef std::vector< TBuffer >		TBufferArray;	// array of buffers

	int const idParent;
	LoggerMPI & logger;
	Communicator & comm;
	std::vector<MPI::Request> requests;
	TBufferArray asyncBuffers;		// asynchronous send buffers; size = no. work processes
	int sendCount;				// number of sends == number of destination processes

	int PackData (				// returns buffer, size
	  StrVec const & strVec,
	  int const numCmdLines,
	  TBuffer & buffer);

	void PrepareData (
	  StrVec & strVec,			///< strings to send
	  int & numChars);			///< returns sum of strVec sizes

	// unused functions
	CommStrings (CommStrings const & rhs);
	CommStrings & operator= (CommStrings const & rhs);
	bool operator== (CommStrings const &rhs) const;
	bool operator!= (CommStrings const & rhs) const;

	/// @endcond
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_CommStrings_h
