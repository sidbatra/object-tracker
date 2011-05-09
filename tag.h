/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    tag.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Delcares a data structure for holding information about a user defined tag on a video
*****************************************************************************/

#pragma once

#include "base.h"
#include "frame.h"

class Tag
{
 private:
    vector<Position*> _allPos;
    vector<Position*> _pos;
    IplImage* _cutout;
   
    string _imagePath;
    string _xmlPath;

    Position _initialPos;
    bool _isDone;

 public:
    
	Tag();
    Tag( string info , float fps , int videoWidth , int videoHeight , string imageFolder , string xmlFolder , int index);
	Tag(const Tag& t);

	bool isValid(int frameID , bool isReverse);
	bool isInitPos(int frameID);
	void setDone();
	
	void generateCutout( Frame *frame );
	
	vector<Position*>& getPos();
	void addPos(Position *p);
	void initPos();
	void refreshPos();
	
	void write(int videoWidth , int videoHeight);
	
    void free();
    void freePos();
    void freeAllPos();
    
};

