/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    point.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Function definitions for the point data structure in point.h
*****************************************************************************/

#include "point.h"

using namespace std;
    
//****************************************************************************
//Constructor Logic
//****************************************************************************
    
Point::Point() 
{
   	x = 0.0;
   	y = 0.0;
}
    
Point::Point(double v) 
{
	x = v; y = v;
}

     
Point::Point(double x_p, double y_p) 
{
	x = x_p; y = y_p;
}
    
Point::Point(const Point& p) 
{
	x = p.x; y = p.y;
}

//****************************************************************************
//Method
//****************************************************************************

bool Point::isOutOfBounds( IplImage *image)
{
	return !(x >= 0 && x < image->width && y >= 0 && y < image->height);
}

void Point::normalize(double oldWScale , double newWScale , double oldHScale , double newHScale )
{
	x = (int)( (x * newWScale) / oldWScale ); 
	y = (int)( (y * newHScale) / oldHScale );
}
   
std::ostream& operator<<(std::ostream& os, const Point& p)
{
	os << p.x << " " << p.y;
	return os;
}

std::istream& operator>>(std::istream& is, Point& p)
{
	is >> p.x >> p.y;
	return is;
}
