/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Logger.cpp
 \date     30-08-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	This file contains the decleration of the logger class.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream>


/*!******************************************************************************************************************
*   NOTES (For jw, dw bout it):
*   
*   Header only vs normal(.cpp file)
* 
*   Header only needs to be compiled as many times as it get included,
*   instancing could get weird?(highly likely we would need multiple instances)
* 
*   Most disadvantages (of header only) point towards compile times, everytime
*   the header file's modified, all dependencies are recompiled and re-linked.
* 
*   srcs: 
*   https://softwareengineering.stackexchange.com/questions/305618/are-header-only-libraries-more-efficient
*   https://stackoverflow.com/questions/12671383/benefits-of-header-only-libraries
* 
* 
*   Tentative format:
*   "[hh:mm::ss]<INSTANCE><TYPE(FATAL_ERROR/ERROR/MESSAGE)><FROM><MESSAGE>"
*   e.g.
*   Message example: "[19:03:05]<ENGINE><MESSAGE><UpdateTargetFPSBasedOnKey()><KEY_6 was pressed, FPS target changed to 180>"
*   Error example:   "[19:03:06]<ENGINE><ERROR><UpdateTargetFPSBasedOnKey()><KEY_G was pressed, FPS target reset to 60>"
*   Convention for the message format
* 
*********************************************************************************************************************/


class Logger
{
/* ----- Public variables ----- */
public:
    using LoggerFlag = unsigned short;    // 16 bits, 16 flags, can expand later, unlikely
    enum EnumLoggerFlags : LoggerFlag
    {
        // Modes flags
        RELEASE            = 0b0000000000000000,      // ONLY critical (crashes/fatal events) information will be logged
        DEBUG              = 0b0000000000000001,      // ALL information will be logged 
        // Misc flags
        WRITE_TO_CONSOLE   = 0b0000000000000010,      // enable/disable write to console *edit, maybe can just take the debug flag?
        WRITE_TO_FILE      = 0b0000000000000100,      // enable/disable write to file
        NUM_FLAGS
    };
    const std::string instanceName;   // name of the current instance

/* ----- Constructors ----- */
public:
    /*!***********************************************************************************
     \brief Construct a new Logger object
     
     \param[in] inst_name The name of the instance to create with
    *************************************************************************************/
    Logger(const char inst_name[]);

    /*!***********************************************************************************
     \brief Destroy the Logger object, also flushes the log before ending the instance
     
    *************************************************************************************/
    ~Logger()
    {
        FlushLog();
        if (m_outFile.is_open())
            m_outFile.close();
    }
    
/* ----- Public setters ----- */
public:
    /*!***********************************************************************************
     \brief Sets the specified flags to allow changes in the behaviour of the logger
            Refer to EnumLoggerFlags for all currently supported flags
     
     \param[in] r_flag  The flags to modify.
     \param[in] r_set   The state to set the flags to,
                        true  = set specified flags to true,
                        false = set specified falgs to false
    *************************************************************************************/
    void SetFlag(const LoggerFlag& r_flag, const bool& r_set);
    
    /*!***********************************************************************************
     \brief  Sets a static variable within the class to the current system clock time
    		 this is to prevent unnecessary calls to get said time multiple times per
    		 frame.
    
    *************************************************************************************/
    void SetTime();

/* ----- Public Methods ----- */
public:
    /*!***********************************************************************************
     \brief Adds a log message (or error) to the log buffer.
            In the even it is an error, it will automatically flush the log.
            
     
     \param[in] r_isError   Sepcifies if the message is an error,
                            true = error,
                            false = message
     \param[in] r_msg       The message to log
     \param[in] fnName      The name of the function calling the log
                            use: __FUNCTION__ OR __FUNCDNAME__
                            The former is just the function name without decoration, use in case of 
                            the function only having one instance.
                            The latter is with decoration, it allows us to defferentiate between which
                            instance it was called within.
    *************************************************************************************/
    void AddLog(const bool& r_isError, const std::string& r_msg, const char fnName[]);
    
    /*!***********************************************************************************
     \brief Flushes the whole log into console or to a file as specified by flags
     
    *************************************************************************************/
    void FlushLog();

/* ----- Private variables----- */
private:
    std::stringstream m_logBuffer;       // all instances will share the same buffer? or sperate them
                                                // the buffer will hold all lines until it is flushed to console/file?
                                                // after which it is cleared. (e.g. wait till end of each frame before flushing?)
    static LoggerFlag m_flags;                  // mode singleton (all instances should share the same mode)
    std::string m_currTime;
    std::ofstream m_outFile;
};
