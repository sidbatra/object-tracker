/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    tag.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions in tag.h
*****************************************************************************/

#include "tag.h"

//****************************************************************************
//Constructor Logic
//****************************************************************************
	
Tag::Tag()
{
    _cutout = NULL;
    _isDone = false;
}



Tag::Tag( string info , float fps , int videoWidth , int videoHeight , string imageFolder , string xmlFolder , int index)
{
    _cutout = NULL;
    _isDone = false;

    vector<string> data;
    Utilities::split(info, data , string(" "));

    _initialPos = Position(atof(data[0].c_str()),atof(data[1].c_str()),atof(data[2].c_str()),atof(data[3].c_str()),Utilities::roundOff( (atoi(data[4].c_str()) * fps) / Constants::TIME_STAMP_SCALE ) );
    _initialPos.normalize(Constants::SPATIAL_SCALE,videoWidth,Constants::SPATIAL_SCALE,videoHeight);
    
    _xmlPath = xmlFolder + "/" + Utilities::toString(index) + Constants::TAG_XML_EXT ;
    _imagePath =  imageFolder + "/" + Utilities::toString(index) + Constants::TAG_IMG_EXT; 
    
    _allPos.push_back( new Position(_initialPos));
    
	initPos();

}

Tag::Tag(const Tag& t)
{

	LOG( LOG_DEBUG , "Tag copy constructor\n");
		
    copy(t._pos.begin(),t._pos.end(),_pos.begin());
    copy(t._allPos.begin(),t._allPos.end(),_allPos.begin());
    t._cutout ?  cvReleaseImage(&_cutout) ,  _cutout = cvCloneImage(t._cutout) : _cutout = NULL ;

    _imagePath = t._imagePath;
    _xmlPath = t._xmlPath;
    _isDone = t._isDone;

    _initialPos = Position(t._initialPos.loc,t._initialPos.width,t._initialPos.height,t._initialPos.frame);
}
    


//****************************************************************************
//Methods
//****************************************************************************

void Tag::addPos(Position *p)
{
	_allPos.push_back( new Position(*p) );
	
	delete _pos[0];
	_pos.erase(_pos.begin());
	_pos.push_back( new Position(*p) );

	delete p;

}

void Tag::generateCutout(Frame* frame)
{
	if( _cutout == NULL )
		_cutout = frame->generateCutout(_initialPos);
}

void Tag::initPos()
{

	for( unsigned p=1 ; p<= Constants::FRAME_VOLUME / 2  ; p++ )
    	_pos.push_back( new Position() );
    
    _pos.push_back( new Position(_initialPos) );
	
}

void Tag::refreshPos()
{
	_isDone = false;
	freePos();
	initPos();
}

vector<Position*>& Tag::getPos()
{	
	return _pos;
}


void Tag::setDone()
{
	_isDone = true;
}

bool Tag::isInitPos(int frameID)
{
	return _initialPos.frame == frameID;
}

bool Tag::isValid(int frameID , bool isReverse)
{
	int diff = _initialPos.frame - frameID;
	//return  (isReverse ?  diff >= 0 && diff < 300 : diff <= 0 && diff > -300) && !_isDone ;
	return  (isReverse ?  diff >= 0 : diff <= 0 ) && !_isDone ;
}

void Tag::write(int videoWidth , int videoHeight)
{
	
	LOG( LOG_VERBOSE , "Writing tag ");

	if( _cutout != NULL )	
		cvSaveImage( _imagePath.c_str() , _cutout);

	ofstream out(_xmlPath.c_str() , ios::out);
	
	out<<"<tag>\n";
	
	for( unsigned p=0 ; p<_allPos.size() ; p++ )
	{
		_allPos[p]->normalize(videoWidth,Constants::SPATIAL_SCALE,videoHeight,Constants::SPATIAL_SCALE);
		out<<_allPos[p]->xmlString();
	}
	
	out<<"</tag>";
	
	out.close();
	
	LOG( LOG_VERBOSE , "Done writing tag\n");
}

void Tag::freePos()
{
	for( unsigned p=0 ; p<_pos.size() ; p++)
		delete _pos[p];
		
	_pos.clear();		
}

void Tag::freeAllPos()
{
	for( unsigned p=0 ; p<_allPos.size() ; p++)
		delete _allPos[p];
		

	_allPos.clear();
}


void Tag::free()
{

	freeAllPos();
	freePos();

    cvReleaseImage(&_cutout);
}
