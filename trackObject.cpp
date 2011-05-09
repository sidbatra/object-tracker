/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    trackObject.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** EXE cpp that controls the interface to the application
*****************************************************************************/

#include "base.h"
#include "video.h"

void usage()
{
    cerr << "USAGE: ./trackObject [OPTIONS] <video path> <cache path> <moment folder> <temp folder> <image folder> <xml folder> <video info> <[tag info]>" << endl;
    cerr << "OPTIONS:" << endl
        << " -v                :: log verbose leel <uint> "<<endl
        << " -l                :: log file <string> "<<endl
	 
	 << endl;
}


int main(int argc, char *argv[])
{
	const int VIDEO_INDEX = 7;	
    const int NUM_REQUIRED_PARAMETERS = 6;
    LogLevel logLevel = LOG_DEBUG;
    LogType logType = LOG_CONSOLE;


    char **args = argv + 1;
    int nargs = argc-1;

	//Setup options given in the arguments	
	for( int a=1 ; a<argc ; a++ )
	{		
		if( !strcmp(argv[a], "-v") )
		{
			logLevel = (LogLevel)atoi( argv[a+1] );			
			args+=2; nargs -=2 ;
		}
		else if( !strcmp(argv[a], "-l") )
		{		
			logType = LOG_FILE;
			Logger::filename = string(argv[a+1]);			
			args+=2; nargs -=2 ;
		}
	}
	
	if ( nargs < NUM_REQUIRED_PARAMETERS )
    {
		usage();
		return -1;
	}
	
	Logger::setup(logLevel,logType);
	
    //Setup argument holding variables
    const char* videoPath = args[0];
    const char* cachePath = args[1];
    const char* momentFolder = args[2];
    const char* tempFolder = args[3];
    const char* imageFolder = args[4];
    const char* xmlFolder = args[5];
    const float fps = atof(args[6]);

	LOG(LOG_MESSAGE,"Video path - "<<videoPath);
	LOG(LOG_MESSAGE,"Cache path - "<<cachePath);
	LOG(LOG_MESSAGE,"Moment folder - "<<momentFolder);
	LOG(LOG_MESSAGE,"Temp folder - "<<tempFolder);
	LOG(LOG_MESSAGE,"Image folder - "<<imageFolder);
	LOG(LOG_MESSAGE,"XML folder - "<<xmlFolder);
	LOG(LOG_MESSAGE,"Video FPS - "<<fps);
	

    vector<string> tagData;
	
	for( int i=VIDEO_INDEX+1  ; i<nargs ; i++)
		tagData.push_back( string(args[i] ) );
		
		
    Video vid(tempFolder, momentFolder , cachePath , videoPath , imageFolder , xmlFolder , fps ,args[VIDEO_INDEX] , tagData);
    vid.extractFrames();
    
    vid.initFrameClusters();
    vid.setupFrameClusters();
    
    vid.trackTags();

    vid.reverse();
    vid.trackTags();
        
    vid.write();
    vid.free();
      
    Logger::free();  
        
    return 0;
}
