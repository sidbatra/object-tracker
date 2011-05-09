/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    frame.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions in frame.h
*****************************************************************************/

#include "frame.h"


//****************************************************************************
// Constructor Logic
//****************************************************************************

Frame::Frame()
{
    _grey = NULL;
    _color = NULL;

    _index = -1;
    _clusterIndex = -1;
}

Frame::Frame( IplImage* image , unsigned index  , unsigned clusterIndex)
{
	_index = index;
	_clusterIndex = clusterIndex;
	
	_color = cvCloneImage(image);
	
	_grey = cvCreateImage( cvGetSize(image) , image->depth , 1);
	cvCvtColor(image,_grey,CV_RGB2GRAY);

}

Frame::Frame( string filename , unsigned index , unsigned clusterIndex)
{
	_index = index;
	_clusterIndex = clusterIndex;

	_color = cvLoadImage( filename.c_str() , CV_LOAD_IMAGE_COLOR );
	
	_grey = cvCreateImage( cvGetSize(_color) , _color->depth , 1);
	cvCvtColor(_color,_grey,CV_RGB2GRAY);

}



Frame::Frame(const Frame& f)
{

	LOG( LOG_DEBUG , "Frame copy constructor\n");
    f._grey ?  cvReleaseImage(&_grey) ,  _grey = cvCloneImage(f._grey) : _grey = NULL ;
    f._color ?  cvReleaseImage(&_color) ,  _color = cvCloneImage(f._color) : _color = NULL ;
    

    _index = f._index;
    _clusterIndex = f._clusterIndex;
}


//****************************************************************************
//Properties
//****************************************************************************

int Frame::getIndex()
{
	return _index;
}

unsigned int Frame::getClusterIndex()
{
	return _clusterIndex;
}


IplImage* Frame::getGreyImage()
{
	return _grey;
}


IplImage* Frame::getColorImage()
{
	return _color;
}

bool Frame::isEmpty()
{
	return _index == -1;
}
    

//****************************************************************************
//Methods
//****************************************************************************

IplImage* Frame::generateThumbnail()
{
	IplImage* thumb = cvCreateImage( cvSize( Constants::THUMBNAIL_WIDTH , (int)( (_color->height* Constants::THUMBNAIL_WIDTH) / _color->width) )  , _color->depth , _color->nChannels);
	cvResize( _color , thumb);
	return thumb;
}

bool Frame::computeNextPosition(Position* p , Position *n , IplImage* vx ,IplImage* vy)
{
	
	bool isDone = false;
	
	if( !p->isOutOfBounds(vx) )
	{
	
		vector<double> mx;
		vector<double> my;
	
		int startY = (int)max( 0.0 ,p->loc.y );
		int startX = (int)max( 0.0 ,p->loc.x );
		int endY = (int)min( vx->height + 0.0 , p->loc.y + p->height - 1 );
		int endX = (int)min( vx->width + 0.0 , p->loc.x + p->width  - 1 );
	
		for( int y= startY ; y<endY ; y++)
			for( int x= startX ; x<endX ; x++)
				{
					mx.push_back( CV_IMAGE_ELEM( vx , float , y,x) );
					my.push_back( CV_IMAGE_ELEM( vy , float , y,x) );				
				}
			
		sort(mx.begin() , mx.end() );
		sort(my.begin() , my.end() );
			
		LOG( LOG_VERBOSE , _index<<" MEDIAN x = "<<mx[mx.size()/2]<<" y = "<<my[my.size()/2]);	

		n->loc.x += mx[mx.size()/2];
		n->loc.y += my[my.size()/2];
	}
	else
		isDone = true;
		
	return false;	
}



IplImage* Frame::generateCutout( Position pos )
{

	IplImage* cutout = NULL;
	

	if( !pos.isOutOfBounds(_color) )
	{

		Position p = pos.getInBoundPosition(_color);

		cutout = cvCreateImage( cvSize((int)p.width , (int)p.height), _color->depth , _color->nChannels);
		cvSetImageROI(_color,p.getRect());
		cvCopyImage( _color , cutout);
		cvResetImageROI(_color);	
	}
	
	
	
	return cutout;
}


void Frame::free()
{
	    cvReleaseImage(&_grey);
	    cvReleaseImage(&_color);	    
}

