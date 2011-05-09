/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    constants.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Declares constants for the class Constants
*****************************************************************************/

#pragma once

#include "base.h"

class Constants
{

    public:

    //***************** Video **********************
    
    static unsigned FRAME_VOLUME;
    static string FRAME_EXT;
    static string MOMENT_EXT;
	static unsigned FRAME_PADDING;
	static int MASK_SIZE_PRE_CLUSTERING ;
	static int TOTAL_CLUSTERING_FEATURES ;
	static int TOTAL_CLUSTERS;
	static int CLUSTERING_ITERATIONS;
	static double CLUSTERING_ACCURACY;
	static int CLUSTER_SIMILARITY_THRESHOLD ;
	
	//***************** Frame **********************
	static int TOTAL_FEATURES;
	static unsigned int MINIMUM_MATCHES;
	static unsigned int MINIMUM_OVERALL_MATCHES;
	static int THUMBNAIL_WIDTH;
	static int THUMBNAIL_HEIGHT;
	
	
    //***************** Tracker **********************
	
	static CvSize WINDOW_SIZE;
		
    //***************** Logger **********************
	
	static string LOGGER_MESSAGE;
	static string TAG_IMG_EXT ;
	static string TAG_XML_EXT ;
        
    //***************** Misc **********************

    static double PI ;
    static double THRESHOLD_FOR_BLANK_FRAME ;
    static double THRESHOLD_FOR_REPEAT_FRAME ;
    static int THRESHOLD_CUT_SCENE ;
    static bool VERBOSE ;
    static CvSize SPATIAL_IMAGE_SIZE ;
    
    //***************** Flex **********************
    
	static unsigned TIME_STAMP_SCALE;
	static unsigned SPATIAL_SCALE ;

};
