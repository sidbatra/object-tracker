/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    tracker.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions in tracker.h
*****************************************************************************/


#include "tracker.h"


//****************************************************************************
//Constructor Logic
//****************************************************************************
    
Tracker::Tracker() {}

//****************************************************************************
//Methods
//****************************************************************************

Position* Tracker::track( vector<Frame*>& frames , vector<Position*>& pos , bool isReverse , bool &isDone)
{
	Frame* prev = frames[ Constants::FRAME_VOLUME / 2 ];
	Frame* next = frames[ Constants::FRAME_VOLUME / 2  + 1 ];
	
	Position* p = pos[pos.size() -1];	
	Position* n = new Position(*p); n->frame += (isReverse ? -1 : 1);
	
	IplImage* vx = cvCreateImage( cvGetSize(prev->getGreyImage()) , IPL_DEPTH_32F , 1);
	IplImage* vy = cvCreateImage( cvGetSize(prev->getGreyImage()) , IPL_DEPTH_32F , 1);
		
	cvCalcOpticalFlowLK( prev->getGreyImage() , next->getGreyImage() , Constants::WINDOW_SIZE , vx , vy);
				
	isDone = next->computeNextPosition(p, n , vx , vy);	
		
	cvReleaseImage(&vx);
	cvReleaseImage(&vy);
	
	//n->plot( next->getColorImage()  , prev->getIndex() , "/sandbox/temp/");
	
	return n;
}

