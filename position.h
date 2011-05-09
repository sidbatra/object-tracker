/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    position.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Delcares a data structure for holding location, size and frame data
*****************************************************************************/

#pragma once

#include "base.h"

class Position
{
 public:
    Point loc;
    double width;
    double height;
    int frame;

 public:
    
	Position();
	Position(Point loc_p , double w_p , double h_p , int frame_p); 
	Position(double x_p , double y_p  , double w_p , double h_p , int frame_p); 
	Position(const Position& p);

	void plot( IplImage *image  , int index , string debugFolder = "");
	void project(CvMat* A);
	bool contains( float x , float y );
	bool isOutOfBounds( IplImage *image );
	Position getInBoundPosition( IplImage* image );
	CvRect getRect();
	string xmlString();
	void normalize(double oldWScale , double newWScale , double oldHScale , double newHScale );
    
    friend std::ostream& operator<<(std::ostream& os, const Position& p);
    friend std::istream& operator>>(std::istream& is, Position& p);
};

