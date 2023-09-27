/*!*****************************************************************************
    @file       Logger.h
    @author     Foong Jun Wei
    @co-author
    @par        DP email: f.junwei\@digipen.edu
    @par        Course: CSD2401, Section A
    @date       250823

    @brief      This file contains the decleration of the logger class.
                

All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

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
    const std::string m_instanceName;   // name of the current instance

/* ----- Constructors ----- */
public:
    // Methods/Functions
    Logger(const char inst_name[]);
    ~Logger()
    {
        FlushLog();
        if (m_outFile.is_open())
            m_outFile.close();
    }
    
/* ----- Public setters ----- */
public:
    void SetFlag(const LoggerFlag& flag, const bool& set);
    void SetTime();

/* ----- Public Methods ----- */
public:
    void AddLog(const bool& is_error, const std::string& msg, const char fn_name[]);
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

#endif // !LOGGER_H