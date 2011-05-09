/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    point.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Delcares a data structure to handle the working of a Point
*****************************************************************************/

#pragma once

#include <iostream>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

class Point 
{
 public:
    double x;
    double y;

 public:
    
	Point();
	Point(double v);
	Point(double x_p, double y_p); 
	Point(const Point& p);

	void normalize(double oldWScale , double newWScale , double oldHScale , double newHScale );
	bool isOutOfBounds( IplImage *image);
	
    friend std::ostream& operator<<(std::ostream& os, const Point& p);
    friend std::istream& operator>>(std::istream& is, Point& p);
};

