/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    prepareImage.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** EXE cpp that resizes and crops images
*****************************************************************************/

#include "base.h"
#include "video.h"

void usage()
{
    cerr << "USAGE: ./prepareImages [OPTIONS] <temp folder> <image folder> <fps> <video info> <[tag info]>" << endl;
    cerr << "OPTIONS:" << endl
        << " -v                :: log verbose level <uint> "<<endl
        << " -l                :: log file <string> "<<endl
	 
	 << endl;
}

IplImage* createThumb(IplImage* frame);
IplImage* createFocus(IplImage* frame , Position pos );


int main(int argc, char *argv[])
{
	const int VIDEO_INDEX = 3;	
    const int NUM_REQUIRED_PARAMETERS = 2;
    LogLevel logLevel = LOG_DEBUG;
    LogType logType = LOG_CONSOLE;


    char **args = argv + 1;
    int nargs = argc-1;

	//Setup options given in the arguments	
	for( int a=1 ; a<argc ; a++ )
	{		
		if( !strcmp(argv[a], "-v"))
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
    const char* tempFolder = args[0];
    const char* imageFolder = args[1];
    const float fps = atof(args[2]);

	LOG(LOG_MESSAGE,"Temp folder - "<<tempFolder);
	LOG(LOG_MESSAGE,"Image folder - "<<imageFolder);
	LOG(LOG_MESSAGE,"Video FPS - "<<fps);
	
	for( int i=VIDEO_INDEX+1  ; i<nargs ; i++)
	{
		cout<<args[i]<<"\n";
		
		vector<string> data;
	    Utilities::split(string(args[i]), data , string(" "));
	    
	   int index = max( (int)(atof(data[4].c_str()) * fps) , 1);
	   string filename = string(tempFolder) + "/" + Utilities::padZeroes( Utilities::toString(index) , 7 ) + ".jpg";
	   string outfilename = string(imageFolder) + "/" + string(data[6]);
	   cout<<filename<<" "<<outfilename<<"\n";
	   
	   IplImage* frame = cvLoadImage( filename.c_str() , CV_LOAD_IMAGE_COLOR );
	   IplImage* thumb = NULL;
	    
	    if( data[7] == "true" )
	    {
	    	thumb = createThumb(frame);
		}
		else
		{
			Position p( atoi(data[0].c_str()) , atoi(data[1].c_str()) ,atoi(data[2].c_str()) ,atoi(data[3].c_str()) , 0 );
			p.normalize(Constants::SPATIAL_SCALE, frame->width, Constants::SPATIAL_SCALE , frame->height );
			thumb = createFocus(frame , p);
		}
		
		cvSaveImage(outfilename.c_str() , thumb);
		
		
		cvReleaseImage(&frame);
		cvReleaseImage(&thumb);
	
	}//tags


    
    Logger::free();  
        
    return 0;
}


IplImage* createThumb(IplImage* frame)
{
	//IplImage* thumb = cvCreateImage( cvSize( Constants::THUMBNAIL_WIDTH , (int)( (frame->height* Constants::THUMBNAIL_WIDTH) / frame->width) )  , frame->depth , frame->nChannels);
	IplImage* thumb = cvCreateImage( cvSize( min( (int)( (frame->width * Constants::THUMBNAIL_HEIGHT) / frame->height) , Constants::THUMBNAIL_WIDTH) , Constants::THUMBNAIL_HEIGHT )  , frame->depth , frame->nChannels );
	cvResize( frame , thumb);
	return thumb;
}

IplImage* createFocus(IplImage* frame , Position pos )
{
	
	IplImage* cutout = NULL;
	
	if( !pos.isOutOfBounds(frame) )
	{
		Position p = pos.getInBoundPosition(frame);
		
        cutout = cvCreateImage( cvSize((int)p.width , (int)p.height), frame->depth , frame->nChannels);
		cvSetImageROI(frame,p.getRect());
		cvCopyImage( frame , cutout);
		cvResetImageROI(frame);	
	}
    else
    {
	    cutout = createThumb(frame);
    }
		
	return cutout;
}


