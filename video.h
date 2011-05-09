/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    video.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Delcares a data structure for encapsulating the functinality of a video
*****************************************************************************/

#pragma once

#include "base.h"
#include "frame.h"
#include "tag.h"
#include "tracker.h"

class Video
{
 private:
    vector<Frame*> _frames;
    vector<Tag*> _tags;
	vector<IplImage*> _miniFrames;
    
    Tracker _tracker;
    CvCapture* _stream;
    CvMat* _featureMatrix;
    vector<unsigned int> _clusterIndex;
    vector<unsigned int> _clusterCount;
    vector<unsigned int> _currentClusterCount;

	string _tempFolder;
    string _xmlFolder;
    string _imageFolder;
    string _momentFolder;
    string _cachePath;

	bool _isReverse;
    bool _isProcessed;
    float _fps;
    int _height;
    int _width;
    unsigned _totalFrames;


 public:
    
	Video();
    Video(const char* tempFolder, const char* momentFolder, const char* cachePath ,const char* videoPath , const char* imageFolder , const char *xmlFolder , float fps ,  const char* info, vector<string> tagData);

	bool prepareFrames(unsigned frameID);
	string frameFilename(unsigned frameID);
	string momentFilename(unsigned frameID);
	string clusterFilename(unsigned frameID , unsigned clusterID);
	
	void insertInFeatureMatrix(IplImage *mini , int index);
	vector<float> computeClusteringFeatures(IplImage* mini );
	void initFrameClusters();
	void setupFrameClusters();

	void saveMoment();
	void reverse();
	void extractFrames();
	void trackTags();
	void write();
	void writeClusterIndex();
	void readClusterIndex();
	void initFrames();
	
	bool testCutScene();
	
    void free();
    void freeFrames();
    void freeTags();
    
};

