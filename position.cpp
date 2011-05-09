/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    position.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions delcared in position.h
*****************************************************************************/


#include "position.h"

//****************************************************************************
//Constructor Logic
//****************************************************************************
    
Position::Position(){frame = -1;};

Position::Position(Point loc_p , double w_p , double h_p , int frame_p)
{
    loc.x = loc_p.x;
    loc.y = loc_p.y;
    width = w_p;
    height = h_p;
    frame = frame_p; 
}

Position::Position(double x_p , double y_p  , double w_p , double h_p , int frame_p)
{

    loc.x = x_p;
    loc.y = y_p;
    width = w_p;
    height = h_p;
    frame = frame_p; 
}


Position::Position(const Position& p)
{
    loc.x = p.loc.x;
    loc.y = p.loc.y;
    width = p.width;
    height = p.height;
    frame = p.frame;
}

//****************************************************************************
//Methods
//****************************************************************************

void Position::project(CvMat* A)
{

	double tempX = loc.x + width , tempY = loc.y + height;
	
	VisionUtilities::projectCoordinate( A , loc.x , loc.y );
	VisionUtilities::projectCoordinate( A , tempX , tempY );
	
	width = tempX - loc.x;
	height = tempY - loc.y;

}

bool Position::contains( float x , float y )
{
	return x >= loc.x && x < loc.x + width && y >= loc.y && y < loc.y + height ;
}

bool Position::isOutOfBounds( IplImage* image )
{
	return loc.isOutOfBounds(image) && Point(loc.x + width , loc.y + height).isOutOfBounds(image);
}

Position Position::getInBoundPosition( IplImage* image )
{
	Position p;

	p.loc.x = max( 0.0 ,loc.x );
	p.loc.y = max( 0.0 ,loc.y );
	p.height =  p.loc.y + height >= image->height ?  image->height - p.loc.y : height ;
	p.width  =  p.loc.x + width  >= image->width  ?  image->width  - p.loc.x : width  ;

	p.frame = frame;
	
	return p;
}

void Position::plot( IplImage *image  , int index , string debugFolder )
{
	if( image != NULL )
	{	
		CvScalar color = cvScalar(0,255,0);
		int thickness = 1;
		
		cvLine( image , cvPoint((int)loc.x,(int)loc.y) , cvPoint((int)(loc.x + width) , (int)loc.y)  , color, thickness, CV_AA );
		cvLine( image , cvPoint((int)loc.x,(int)loc.y) , cvPoint((int)loc.x , (int)(loc.y+height))  , color, thickness, CV_AA );
		cvLine( image , cvPoint((int)(loc.x+width),(int)loc.y) , cvPoint((int)(loc.x+width) , (int)(loc.y+height))  , color, thickness, CV_AA );
		cvLine( image , cvPoint((int)loc.x,(int)(loc.y+height)) , cvPoint((int)(loc.x+width) , (int)(loc.y+height))  , color, thickness, CV_AA );

		if( debugFolder == "" )
			Utilities::display(image);
		else
		{
			string filename = debugFolder + string("/") + Utilities::padZeroes(Utilities::toString(index),7) + ".jpg";
			cvSaveImage(filename.c_str() , image);
		}
	}
}

CvRect Position::getRect()
{
	return cvRect( (int)loc.x ,(int)loc.y , (int)width , (int)height);
}

string Position::xmlString()
{
	return "<pos n=\"" + Utilities::toString(frame) + "\" x=\"" + Utilities::toString((int)loc.x) +
			"\" y=\"" + Utilities::toString((int)loc.y) + "\" w=\"" + Utilities::toString((int)width) +
			"\" h=\"" + Utilities::toString((int)height) + "\" />\n";
 }

void Position::normalize(double oldWScale , double newWScale , double oldHScale , double newHScale )
{
	width = Utilities::normalize(width,oldWScale,newWScale);
	height = Utilities::normalize(height,oldHScale,newHScale);  

	loc.normalize(oldWScale,newWScale,oldHScale,newHScale);
}
    
std::ostream& operator<<(std::ostream& os, const Position& p)
{
    os<<p.loc<<" "<<p.width<<" "<<p.height<<" "<<p.frame;
    return os;
}

std::istream& operator>>(std::istream& is, Position& p)
{
    is>>p.loc>>p.width>>p.height>>p.frame;
    return is;
}

