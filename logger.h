/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    logger.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** A set of static methods used for handling application debugging
*****************************************************************************/

#pragma once

#include "base.h"

typedef enum LogLevel {	 LOG_NONE , LOG_FATAL = 0, LOG_ERROR, LOG_WARNING, LOG_MESSAGE, LOG_VERBOSE, LOG_DEBUG };
typedef enum LogType {	LOG_FILE = 0, LOG_CONSOLE };

#define LOG(L, M) {std::stringstream s; s << M; Logger::log(L, s.str());}

class Logger
{
	public:
	
	static LogLevel level;
	static LogType type ;
	static string prefix ;
	static ofstream stream;
	static string filename;
	
	static void setup( LogLevel level_p , LogType type_p );
	static void free();
	static void log( LogLevel level_p , const string &msg);
	        
};

