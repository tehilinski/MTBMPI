/*! ----------------------------------------------------------------------------------------------------------
@file		MsgTags.h
@class		mtbmpi::MsgTags
@brief 		Provides enum of tags indicating type of content of an MPI message.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_MsgTags_h
#define INC_mtbmpi_MsgTags_h

namespace mtbmpi {

    /// Tags which label the type of an MPI message.
    enum MsgTags
    {
	Tag_FIRST = 101,
	Tag_State,			///< contains state of task
	Tag_TaskResults,		///< contains simulation results
	Tag_LogMessage,			///< contains message for log file
	Tag_ErrorMessage,		///< contains error message
	Tag_BlackboardID,		///< request blackboard task ID number
	Tag_ControllerID,		///< request controller task ID number
	Tag_InitializeTask,		///< to task: initialize
	Tag_StartTask,			///< to task: controller starts task after initialization
	Tag_RequestStopTask,		///< to task: stop
	Tag_RequestPauseTask,		///< to task: pause
	Tag_RequestResumeTask,		///< to task: resume
	Tag_RequestCmdLineArgs,		///< want cmd-line args
	Tag_RequestStop,		///< stop process
	Tag_CmdLineArgs,		///< here is cmd-line args
	Tag_RequestConfig,		///< want config data
	Tag_Configuration,		///< here is config data
	Tag_StopBlackboard,		///< to blackboard: stop
	Tag_Confirmation,		///< requesting confirmation
	Tag_Data,			///< contains data for destination
	Tag_Unknown,
	Tag_LAST
    };

    /// Is the message tag valid?
    inline bool IsMsgTagValid ( MsgTags const tag )
    {
	return ( tag > Tag_FIRST && tag < Tag_LAST );
    }

    /// Is the message tag value valid?
    inline bool IsMsgTagValid ( int const tag )
    {
	return ( tag > (int)Tag_FIRST && tag < (int)Tag_LAST );
    }


} // namespace mtbmpi


#endif // INC_mtbmpi_MsgTags_h
