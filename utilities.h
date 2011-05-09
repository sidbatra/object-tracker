/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    utilities.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** A set of static methods which are used throughout the application as utilities
*****************************************************************************/

#pragma once

#include "base.h"

#define SQUARE(x) x*x

class Utilities
{
	public:
	    static std::string toString(int v);     //Converts the given integer to string
        static std::string padZeroes(string text , unsigned length );    //Pads zeroes as prefix to the given string

        static void populateMatrix(CvMat **A  , double *data , int width , int height);      //Populates the given array into the matrix
		
        static int roundOff(double d); //Rounds of floating point to integer
		static void display(IplImage* image,std::string text = "Default" );
        static void split(string str, vector<string>& tokens, string delimiters);
 
		static int normalize(double n , double oldScale , double newScale );
		static void sortWithIndices( vector<double> &data , vector<int> &indices );
        
        static vector<string> loadFilenames(string folderName , string extension , bool appendFoldername = false, bool isReverse = false );

        static void resizeInPlace(IplImage **image, double scale);
        static void resizeInPlace(IplImage **image, int height, int width);
        static double squareDistance( Point a , Point b);

        static void createDirectory(string path);
        static void moveFile(string source , string destination);
        static void copyFile(string source , string destination);
        static bool doesFileExist(string path);
                
        
};

