/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Logger.cpp
 \date     30-08-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief 	This file contains the defenition of the logger class.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "Logger.h"

#ifndef GAMERELEASE
#include "Editor/Editor.h"
#endif // !GAMERELEASE

#pragma warning(disable : 4996)

// static variables decleration
Logger::LoggerFlag Logger::m_flags;
bool LOG = true;


Logger::Logger(const char inst_name[]) :
	instanceName{ inst_name }
{
#ifndef GAMERELEASE

	SetTime();
	std::string filepath = "../Logs/";
	if (CreateDirectoryA(filepath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		filepath += instanceName;
		filepath += "-";
		filepath += m_currTime;
		filepath += ".log";
		std::replace(filepath.begin(), filepath.end(), ':', '-');
		std::replace(filepath.begin(), filepath.end(), ' ', '_');

		m_outFile.open(filepath);
	}
	else
	{
		throw;
	}
#endif // !GAMERELEASE
}

void Logger::SetFlag(const LoggerFlag& r_flag, const bool& r_set)
{
	// multiple flags being set off should work aka:
	m_flags = (r_set) ? (m_flags | r_flag) : (m_flags & (~r_flag));
}

void Logger::AddLog(const bool& r_isError, const std::string& r_msg, const char fnName[])
{
#ifdef GAMERELEASE
	return;
#endif // GAMERELEASE

	const std::string type = (r_isError) ? "ERROR" : "MESSAGE";
	m_logBuffer << "[" << m_currTime << "]"
				<< "<" << instanceName << ">"
				<< "<" << type << ">"
				<< "<" << fnName << ">"
				<< "<" << r_msg << ">\n";
	if (r_isError)
	{
		SetFlag(DEBUG | WRITE_TO_FILE, true);
		FlushLog();
		m_outFile.close();
	}
}

void Logger::FlushLog()
{
#ifdef GAMERELEASE
	return;
#endif // GAMERELEASE
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
		if (m_flags & EnumLoggerFlags::WRITE_TO_FILE)
			m_outFile << buf << "\n";
	}
	// this clear is required as techincally eof is a error flag, this clears it
	m_logBuffer.clear();
}


void Logger::SetTime()
{
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream tm;
	tm << std::put_time(localtime(&now), "%F %T");
	m_currTime = tm.str();
}


