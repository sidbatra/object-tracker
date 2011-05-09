/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    tracker.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Encapsulation for object tracking related operations
*****************************************************************************/

#pragma once

#include "base.h"
#include "frame.h"
#include "tag.h"

class Tracker
{
 private:


 public:
    
	Tracker();

	Position* track( vector<Frame*>& frames , vector<Position*>& pos , bool isReverse , bool &isDone);
    
};

