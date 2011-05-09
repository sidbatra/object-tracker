/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    constants.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Defines values of constants declared in constants.h
*****************************************************************************/

#include "constants.h"

//***************** Tag **********************
string Constants::TAG_IMG_EXT = ".jpg";
string Constants::TAG_XML_EXT = ".xml";

//***************** Frame **********************
int Constants::TOTAL_FEATURES = 500;
unsigned int Constants::MINIMUM_MATCHES = 9;
unsigned int Constants::MINIMUM_OVERALL_MATCHES = 300;
int Constants::THUMBNAIL_WIDTH = 150;
int Constants::THUMBNAIL_HEIGHT = 84;


//***************** Video **********************
unsigned Constants::FRAME_VOLUME = 5;
string Constants::FRAME_EXT = ".jpg";
string Constants::MOMENT_EXT = ".jpg";
unsigned Constants::FRAME_PADDING = 7;
int Constants::TOTAL_CLUSTERING_FEATURES = 32*32*3 + 74 ;
int Constants::MASK_SIZE_PRE_CLUSTERING = 7;
int Constants::TOTAL_CLUSTERS =  20;// 35;
int Constants::CLUSTERING_ITERATIONS = 50;
double Constants::CLUSTERING_ACCURACY = 1;
int Constants::CLUSTER_SIMILARITY_THRESHOLD = 1000000;


//***************** Tracker **********************
CvSize Constants::WINDOW_SIZE = cvSize(13,13);

//***************** Logger **********************

string Constants::LOGGER_MESSAGE = "\n\n ---------- APP LOG --------------- \n\n";


//***************** Misc **********************

double Constants::PI = 3.14159265358979323846;
double Constants::THRESHOLD_FOR_REPEAT_FRAME = 1.2;
double Constants::THRESHOLD_FOR_BLANK_FRAME = 1;
int Constants::THRESHOLD_CUT_SCENE = 1000;
bool Constants::VERBOSE = false;
CvSize Constants::SPATIAL_IMAGE_SIZE = cvSize(32,32);

//***************** Flex **********************

unsigned Constants::TIME_STAMP_SCALE = 10000;
unsigned Constants::SPATIAL_SCALE = 10000;
