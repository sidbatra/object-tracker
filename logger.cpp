/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    logger.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions of the functions in logger.h
*****************************************************************************/

#include "logger.h"

//****************************************************************************
//Data Members
//****************************************************************************

LogLevel Logger::level;
LogType  Logger::type;
ofstream Logger::stream;
string Logger::filename = "log.txt";
string Logger::prefix = "XEWMVD";

//****************************************************************************
//Methods
//****************************************************************************
	
void Logger::setup( LogLevel level_p , LogType type_p ) {
	Logger::level = level_p;
	Logger::type = type_p;
	
	if(	Logger::type == LOG_FILE && !stream.is_open() )
	{
		Logger::stream.open(Logger::filename.c_str() , ios::out);
		stream<<Constants::LOGGER_MESSAGE;
	}
	else
		cout<< Constants::LOGGER_MESSAGE;
		
	
}

void Logger::free() {
		if(	Logger::type == LOG_FILE && stream.is_open() )
			stream.close();
}

//Submit a log entry
void Logger::log( LogLevel level_p , const string &msg) {

	if( Logger::level >= level_p )
	{
		string out = (level_p == LOG_FATAL ? "(" + string(__FILE__) + " " + Utilities::toString(__LINE__) + ") " : "" ) + Logger::prefix[level_p] + " - " + msg + "\n" ;
		Logger::type == LOG_FILE ? Logger::stream<<out : cout<<out.c_str();
		
		if( level_p == LOG_FATAL )
			abort();
	}
}
	        


