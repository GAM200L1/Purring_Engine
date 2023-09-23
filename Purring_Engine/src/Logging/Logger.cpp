/*!*****************************************************************************
	@file       Logger.cpp
	@author     Foong Jun Wei
	@co-author
	@par        DP email: f.junwei\@digipen.edu
	@par        Course: CSD2401, Section A
	@date       250823

	@brief      This file contains the defenition of the logger class.


All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*******************************************************************************/
#include "prpch.h"
#include "Logger.h"
#pragma warning(disable : 4996)

// static variables decleration
Logger::LoggerFlag Logger::m_flags;
std::stringstream Logger::m_logBuffer;
std::string Logger::m_currTime;
bool LOG = true;

/*-----------------------------------------------------------------------------
/// <summary>
/// Sets the specified flags to allow changes in the behaviour of the logger
///	Refer to EnumLoggerFlags for all currently supported flags
/// </summary>
/// <param name="flag">
/// The flags to modify.
/// </param>
/// <param name="set">
///	The state to set the flags to.
///	true  = set specified flags to true.
/// false = set specified falgs to false.
/// </param>
----------------------------------------------------------------------------- */
void Logger::SetFlag(const LoggerFlag& flag, const bool& set)
{
	// multiple flags being set off should work aka:
	// SetFlag(RELEASE | WRITE_TO_FILE, True)
	m_flags = (set) ? (m_flags | flag) : (m_flags & (~flag));
}

/*-----------------------------------------------------------------------------
/// <summary>
/// Adds a message or error to the log buffer object.
/// </summary>
/// <param name="is_error">
/// specifies if the message is an error or not.
///	true  = error
/// false = message
///	</param>
/// <param name="msg">
///	The message to log
/// </param>
/// <param name="fn_name">
///	The name of the function calling the log
/// use: __FUNCTION__ OR __FUNCDNAME__
/// The former is just the function name without decoration, use in case of 
/// the function only having one instance.
/// The latter is with decoration, it allows us to defferentiate between which
/// instance it was called within.
/// </param>
----------------------------------------------------------------------------- */
void Logger::AddLog(const bool& is_error, const std::string& msg, const char fn_name[])
{
	const std::string type = (is_error) ? "ERROR" : "MESSAGE";
	m_logBuffer << "[" << m_currTime << "]"
				<< "<" << m_instanceName << ">"
				<< "<" << type << ">"
				<< "<" << fn_name << ">"
				<< "<" << msg << ">\n";
}

/*-----------------------------------------------------------------------------
/// <summary>
/// Flushes the whole log into console or to a file as specified by flags
/// </summary>
----------------------------------------------------------------------------- */
void Logger::FlushLog()
{
	// if log buffer is empty, return
	if (m_logBuffer.str().empty() || !LOG)
		return;
	std::string buf;
	// do stuff
	while (std::getline(m_logBuffer, buf))
	{
		// if write to console
		if (m_flags & EnumLoggerFlags::WRITE_TO_CONSOLE)
			std::cout << buf << "\n";
		// if write to file

	}

	// this clear is required as techincally eof is a error flag, this clears it
	m_logBuffer.clear();
}

/*-----------------------------------------------------------------------------
/// <summary>
/// Sets a static variable within the class to the current system clock time
/// this is to prevent unnecessary calls to get said time multiple times per
/// frame.
/// </summary>
----------------------------------------------------------------------------- */
void Logger::SetTime()
{
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream tm;
	tm << std::put_time(localtime(&now), "%F %T");
	m_currTime = tm.str();
}


