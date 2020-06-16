/*------------------------------------------------------------------------------------------------------------
file		CommStrings.cpp
class		mtbmpi::CommStrings
brief 		Sends an array of strings to specified tasks, using the specified MPI communicator.
example		../tests/Test_CommStrings.cpp
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "CommStrings.h"
#include "MsgTags.h"
#include "Master.h"
#include <stdexcept>
#include <sstream>
#include <cassert>

// DEBUGGING
// #define DEBUG_CommStrings 1

#ifdef DEBUG_CommStrings
  #include <iostream>
  using std::cout;
  using std::endl;
#endif

// for mpi versions that do NOT have exceptions enabled, define this:
#if defined(OMPI_HAVE_CXX_EXCEPTION_SUPPORT)
  #undef CommStrings_DO_ERROR_CHECKS
#elif defined(MPICH)
  #undef CommStrings_DO_ERROR_CHECKS
#else
  #define CommStrings_DO_ERROR_CHECKS
#endif

inline std::string MakeMyName (
    std::string const className,
    unsigned int const id)
{
    std::ostringstream oss;
    oss << "task " << id << ": " << className << ": ";
    return oss.str();
}


namespace mtbmpi {


char const CommStrings::lineDelimiter = NL_CHAR;


CommStrings::CommStrings (
    int const parentID,
    LoggerMPI & useLogger,
    Communicator & useComm )
    : idParent     ( parentID ),
      logger       ( useLogger ),
      comm         ( useComm ),
      asyncBuffers ( comm.GetComm().Get_size(), TBuffer() ),
      sendCount    ( 0 )
{
}

CommStrings::~CommStrings ()
{
    // no longer need to save the send buffers
    asyncBuffers.clear();
}

/// @cond SKIP_PRIVATE

void CommStrings::PrepareData (
    StrVec & strVec,		// strings to send
    int & numChars)		// returns sum of strVec sizes
{
    numChars = 0;
    // append delimiter to lines so destination can split
    StrVec::iterator iSV = strVec.begin();
    while ( iSV != strVec.end() )
    {
	iSV->push_back ( CommStrings::lineDelimiter );
	numChars += iSV->size();
	++iSV;
    }
}

int CommStrings::PackData (
    StrVec const & strVec,
    int const numCharsInArray,
    TBuffer & buffer)
{
    std::string const myName = MakeMyName( "CommStrings::PackData", idParent );
    #ifdef DEBUG_CommStrings
    {
	std::ostringstream oss;
	oss << myName
	     << "parent task ID = " << idParent
	     << ": packing data"
	     << ": number of lines = " << strVec.size()
	     << ": # chars = " << numCharsInArray;
	logger.Message( oss.str() );
    }
    #endif

    // make an buffer to hold packed data
    int const sizePacked = MPI::CHAR.Pack_size( numCharsInArray, comm.GetComm() );
    assert( sizePacked >= numCharsInArray );
    buffer.second = sizePacked;
    TBufferData & bufferData = buffer.first;
    bufferData.assign( sizePacked, NULL_CHAR );
    // add data to bufferData
    int position = 0;
    StrVec::const_iterator iSV = strVec.begin();
    while ( iSV != strVec.end() )
    {
	//----- DEBUG START
//	std::string const findThis = "nc,10291";
//	std::string const line = *iSV;
//	if ( line.find( findThis ) != std::string::npos )
//	{
//	    std::ostringstream oss;
//	    oss << myName << "DEBUG found \"" << findThis << "\"\n  Line = " << line
//	    	<< "\n  line length = " << line.size()
//	    	<< "\n  Packing buffer Pack_size = " << sizePacked;
//	    logger.Message( oss.str() );
//	}
	//----- DEBUG END

	MPI::CHAR.Pack(
	    iSV->data(), iSV->size(),		// pack this
	    &bufferData[0], sizePacked,		// store here
	    position,				// where in buf
	    comm.GetComm() );			// communicator
	CheckErrorMPI( myName );
	#ifdef CommStrings_DO_ERROR_CHECKS
	  // doesn't work on crayxt
	  if ( position == 0 )     // oops
	  {
	      std::ostringstream oss;
	      oss << myName
		  << "parent task ID = " << idParent
		  << ": MPI::CHAR.Pack error msg: " << GetErrorString();
	      throw std::runtime_error( oss.str() );
	  }
	#else
	  // hopefully exception is thrown upon error
	#endif
	++iSV;
    }
    assert(position <= sizePacked);
    return position;
}

/// @endcond

void CommStrings::Isend (
    IDNum const destinationID,		// destination task ID
    MsgTags const msgTag,		// tag ID for message
    StrVec & strVec )			// strings to send
{
    // send buffer
    std::string const myName = MakeMyName( "CommStrings::Isend", idParent );
    #ifdef DEBUG_CommStrings
    {
	std::ostringstream oss;
	oss << myName << "enter"
	     << ": destination ID = " << destinationID
	     << ": no. lines = " << strVec.size()
	     << ": msg tag = " << msgTag
	     << ": lines =\n" << strVec;
	logger.Message( oss.str() );
    }
    #endif

    // Pack the data into an MPI::PACKED buffer
    // first = buffer data, second = size

    int numCharsInArray = 0;
    PrepareData( strVec, numCharsInArray );

    // store buffer until WaitAll is done
    //asyncBuffers.push_back( TBuffer() );
    //TBuffer & buffer = asyncBuffers.back();
    TBuffer & buffer = asyncBuffers[ sendCount ];
    int const packedSize = PackData( strVec, numCharsInArray, buffer );
    assert( packedSize >= numCharsInArray );

    #ifdef DEBUG_CommStrings
	std::ostringstream oss;
	oss << myName << "enter"
	     << ": destination ID = " << destinationID
	     << ": buffer size (bytes) = " << buffer.second;
	logger.Message( oss.str() );
    #endif

    MPI::Request newRequest =
	comm.GetComm().Isend( buffer.first.data(), buffer.second, MPI::PACKED, destinationID, msgTag );
    requests.push_back( newRequest );
    CheckErrorMPI( myName );	// error check for when MPI exceptions are turned off
    ++sendCount;

    #ifdef DEBUG_CommStrings
    {
	std::ostringstream oss;
	oss << myName << "done"
	     << ": destination ID = " << destinationID
	     << ": msg tag = " << msgTag;
	logger.Message( oss.str() );
    }
    #endif
}

void CommStrings::Receive (
    IDNum const sourceID,
    MsgTags const msgTag,
    StrVec & strVec )
{
    std::string const myName = MakeMyName( "CommStrings::Receive", idParent );
    #ifdef DEBUG_CommStrings
    {
	std::ostringstream oss;
	oss << myName << "enter: "
	     << ": source ID = " << sourceID
	     << ": msg tag = " << msgTag;
	logger.Message( oss.str() );
    }
    #endif

    // error checks
    if ( !IsMsgTagValid( msgTag ) )
    {
	std::ostringstream oss;
	oss << myName
	     << ": source ID = " << sourceID
	     << ": invalid message tag";
	logger.Error( oss.str() );
      	return;
    }

    // wait for message to arrive
    #ifdef DEBUG_CommStrings
    {
	std::ostringstream oss;
	oss << myName
	    << ": source ID = " << sourceID
	    << ": comm.Probe";
	logger.Message( oss.str() );
    }
    #endif
    MPI::Status status;
    comm.GetComm().Probe( sourceID, msgTag, status );
    //int const sizePacked = status.Get_count(MPI::PACKED);
    int const sizePacked = status.Get_count(MPI::CHAR);
    int const maxLineLength = 2048;      // max size of cmd-line expected
    // unpack message
    if ( sizePacked > 0 )
    {
	TBufferData buffer ( sizePacked, NULL_CHAR );
	comm.GetComm().Recv( &buffer[0], buffer.size(), MPI::CHAR, status.Get_source(), status.Get_tag() );
	CheckErrorMPI( myName );	// error check for when MPI exceptions are turned off
	#ifdef DEBUG_CommStrings
	{
	    std::ostringstream oss;
	    oss << myName
		<< ": source ID = " << sourceID
		<< ": Recv finished. Size = " << sizePacked;
	    logger.Message( oss.str() );
	}
	#endif

	int posPacked = 0;
	int posUnpacked = 0;
	TBufferData bufUnpacked (maxLineLength, NULL_CHAR);
	while ( posPacked < sizePacked && posUnpacked < maxLineLength )
	{
	    MPI::CHAR.Unpack (
		    buffer.data(), sizePacked,		// packed data
		    &bufUnpacked[posUnpacked], 1,	// unpacked data, 1 char per iter
		    posPacked,				// where in buffer
		    comm.GetComm() );			// communicator

	    // extract string
	    TBufferData::iterator iNow = bufUnpacked.begin() + posUnpacked;
	    bool const atEnd = ( posPacked == sizePacked || posUnpacked == maxLineLength );
	    if ( *iNow == lineDelimiter || atEnd )
	    {
		// store cmd-line list in config
		std::string const line ( bufUnpacked.begin(), iNow );
		#ifdef DEBUG_CommStrings
		{
		    std::ostringstream oss;
		    oss << myName
			 << ": source ID = " << sourceID
			 << ": new line = " << line;
		    logger.Message( oss.str() );
		}
		#endif
		if ( !line.empty() )
		    strVec.push_back ( line );

		//----- DEBUG START
//		std::string const findThis = "nc,10291";
//		if ( line.find( findThis ) != std::string::npos )
//		{
//		    std::ostringstream oss;
//		    oss << myName
//			<< "DEBUG found \"" << findThis << "\"\n  Line = " << line
//			<< "\n  sizePacked = " << sizePacked
//			<< "  posPacked = " << posPacked
//			<< "  posUnpacked = " << posUnpacked;
//		    logger.Message( oss.str() );
//		}
		//----- DEBUG END

		std::fill ( bufUnpacked.begin(), bufUnpacked.end(), NULL_CHAR );
		posUnpacked = 0;
	    }
	    else // unpack next char
		++posUnpacked;
	}
    }
    else // sizePacked == 0
    {
	comm.GetComm().Recv ( 0, 0, MPI::PACKED, status.Get_source(), status.Get_tag() );
	#ifdef DEBUG_CommStrings
	{
	    std::ostringstream oss;
	    oss << myName
		 << ": source ID = " << sourceID
		 << ": Recv finished. Size = 0";
	    logger.Message( oss.str() );
	}
	#endif
    }
}

void CommStrings::WaitAll ()
{
    std::string const myName = MakeMyName( "CommStrings::WaitAll", idParent );
    if ( !requests.empty() )	// anything to do?
    {
	#ifdef DEBUG_CommStrings
	{
	    std::ostringstream oss;
	    oss << myName << requests.size() << " tasks";
	    logger.Message( oss.str() );
	}
	#endif

	std::vector<MPI::Status> status ( requests.size() );
	MPI::Request::Waitall( requests.size(), requests.data(), status.data() );
	// check for errors
	for ( std::vector<MPI::Status>::const_iterator i = status.begin();
	      i != status.end();
	      ++i )
	{
	    if ( i->Get_error() != MPI_SUCCESS )
	    {
		std::ostringstream oss;
		oss << myName << "send message error."
		    << " Error code: "  << i->Get_error()
		    << " Message tag: " << i->Get_tag()
		    << " Sender rank: " << i->Get_source();
		logger.Error( oss.str() );
	    }
	}
	requests.clear();

	#ifdef DEBUG_CommStrings
	{
	    std::ostringstream oss;
	    oss << myName << "done";
	    logger.Message( oss.str() );
	}
	#endif
    }
}


} // namespace mtbmpi
