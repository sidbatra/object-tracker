/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    video.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions in video.h
*****************************************************************************/

#include "video.h"

//****************************************************************************
//Constructor Logic
//****************************************************************************


Video::Video()
{
    _stream = NULL;
    _featureMatrix = NULL;
    _fps = 0.0;
    _totalFrames = 0;
    _isProcessed = false;
    _isReverse = false;
    _height = 0;
    _width = 0;
}



Video::Video(const char* tempFolder,const char* momentFolder, const char* cachePath , const char* videoPath , const char* imageFolder , const char *xmlFolder , float fps , const char* info , vector<string> tagData)
{

	LOG(LOG_MESSAGE,"Constructing video ");
	
    _stream = cvCaptureFromAVI(videoPath);

    
    !cvGrabFrame(_stream) ? (LOG( LOG_FATAL , "Unable to load video")) : (LOG(LOG_MESSAGE , "Video loaded"));
	

	_tempFolder = string(tempFolder);
    _imageFolder = string(imageFolder);
    _xmlFolder = string(xmlFolder);
    _momentFolder = string(momentFolder);
	_cachePath = string(cachePath);


    _fps = fps;
   	_totalFrames = 0;	
    _isReverse = false;
    _height = (int)cvGetCaptureProperty(_stream,CV_CAP_PROP_FRAME_HEIGHT);
    _width = (int)cvGetCaptureProperty(_stream,CV_CAP_PROP_FRAME_WIDTH);

    vector<string> data;
    Utilities::split(string(info) , data , string(" "));


    _isProcessed = (data[1] == "true");
    

    for( unsigned t=0 ; t<tagData.size() ; t++)
        _tags.push_back( new Tag(tagData[t],fps , _width , _height , _imageFolder , _xmlFolder , t+1) );
        
 	initFrames();
 	
 	LOG(LOG_MESSAGE,"Done constructing video ");
 	
}


//****************************************************************************
//Methods
//****************************************************************************



void Video::insertInFeatureMatrix(IplImage *mini , int index)
{

	vector<float> features = computeClusteringFeatures(mini);
		
	for( unsigned f=0 ;f<features.size() ; f++)
		CV_MAT_ELEM( *_featureMatrix , float , index , f) = features[f];
}

//Computes a vector of features representing the given image for clustering
vector<float> Video::computeClusteringFeatures(IplImage* mini)
{
	vector<float> features = VisionUtilities::computeHistogram(mini,false);
	
	cvCvtColor(mini,mini,CV_RGB2Lab);
	cvSmooth(mini,mini,CV_GAUSSIAN,Constants::MASK_SIZE_PRE_CLUSTERING,Constants::MASK_SIZE_PRE_CLUSTERING);
		
	for( int y=0  ; y<mini->height ; y++ )
		for( int x=0 ; x<mini->width ; x++  )
		{				
			CvScalar lab = VisionUtilities::getRGB(mini,y,x);
			features.push_back(lab.val[0]);
			features.push_back(lab.val[1]);
			features.push_back(lab.val[2]);
		}
	
			
	return features;
}


void Video::initFrameClusters()
{
	LOG(LOG_MESSAGE,"Initializing clustering");

	if( !_isProcessed )
	{
		_featureMatrix = cvCreateMat( _totalFrames , Constants::TOTAL_CLUSTERING_FEATURES ,CV_32FC2 );
		
		for( unsigned int f=0 ; f<_totalFrames ; f++)
		{
			insertInFeatureMatrix(_miniFrames[f],f);
			cvReleaseImage( &_miniFrames[f] );	
		}
		
		_miniFrames.clear();
	}

	_clusterIndex = vector<unsigned int>(_totalFrames);    	
	_clusterCount = vector<unsigned int>(Constants::TOTAL_CLUSTERS);
	_currentClusterCount = vector<unsigned int>(Constants::TOTAL_CLUSTERS);
	
	fill(_clusterCount.begin(),_clusterCount.end(),0);
	fill(_currentClusterCount.begin() , _currentClusterCount.end(),0);
	
	
}


void Video::setupFrameClusters()
{

	if( !_isProcessed )
	{
	
		LOG(LOG_MESSAGE,"Setting up clustering");
		
 		CvMat* clusters = cvCreateMat( _totalFrames , 1, CV_32SC1 );
		
		LOG(LOG_MESSAGE,"Clustering");
		
   	    cvKMeans2( _featureMatrix , Constants::TOTAL_CLUSTERS , clusters, cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, Constants::CLUSTERING_ITERATIONS ,Constants::CLUSTERING_ACCURACY ));
   	    
   	   //--------Compute total frames in each cluster-------

   	   for( unsigned int s=0 ; s<_totalFrames ; s++)
		{
			int index = clusters->data.i[s];
								
			_clusterCount[index]++;
			_clusterIndex[s] = index;
		}
   
   		for( int c=0 ; c<Constants::TOTAL_CLUSTERS ; c++)
	   		LOG(LOG_VERBOSE, c<<" - "<<_clusterCount[c]);
				
		
		//----------------------------------------------------------------
		

		cvReleaseMat(&clusters);
		cvReleaseMat(&_featureMatrix);
		
		LOG(LOG_MESSAGE,"End of clustering");
		
	}
	else
	{
		readClusterIndex();
	}


}


void Video::extractFrames()
{

	LOG(LOG_MESSAGE,"Extracing video frames ");
	
	unsigned f=0;
	
	while(true)
	{
		string filename = frameFilename(f);
		IplImage *frame =  cvRetrieveFrame(_stream);

		cvSaveImage( filename.c_str() , frame);
		
		if( !_isProcessed )
			_miniFrames.push_back(VisionUtilities::computeSpatialImage(frame) );
				
		LOG(LOG_VERBOSE,"Saving image "<<filename);
				
		if( !cvGrabFrame(_stream)  )
			break;

		f++;
	}
	
	_totalFrames = f;

	LOG(LOG_MESSAGE,"Done extracting - "<<_totalFrames<<" video frames");

}



void Video::reverse()
{
	LOG(LOG_MESSAGE,"Reversing video");
	
	_isReverse = true;
	
	freeFrames();
	initFrames();
	
	for( unsigned t=0 ; t<_tags.size() ; t++)
		_tags[t]->refreshPos();
}



string Video::frameFilename(unsigned frameID)
{
	return _tempFolder + "/" + Utilities::padZeroes( Utilities::toString(frameID) , Constants::FRAME_PADDING) + Constants::FRAME_EXT;
}

string Video::momentFilename(unsigned frameID)
{
	return _momentFolder + "/" + Utilities::toString(frameID) + Constants::FRAME_EXT;
}

string Video::clusterFilename(unsigned frameID , unsigned clusterID)
{
	return _momentFolder + "/../clusters/" + Utilities::toString(clusterID) + "/" + Utilities::toString(frameID) + Constants::FRAME_EXT;
}


bool Video::prepareFrames(unsigned frameID)
{
	_frames[0]->free();
	delete _frames[0];
	_frames.erase(_frames.begin() );
	
	_frames.push_back( new Frame( frameFilename(frameID) , frameID , _clusterIndex[frameID] ) );
	
		
	return !_frames[ _frames.size() / 2]->isEmpty();

}


void Video::trackTags()
{

	LOG(LOG_MESSAGE,"Tracking tags");
	
	int diffFrmCenter = _isReverse ? Constants::FRAME_VOLUME / 2  : -(Constants::FRAME_VOLUME / 2);

	
	for( unsigned f=0 ; f<_totalFrames ; f++)
	{
	
		LOG(LOG_DEBUG,"Frame - "<<f);
		
		unsigned fDash = _isReverse ?  _totalFrames - f -1 : f;
			
		if( prepareFrames(fDash) )
		{

			bool isCutScene = testCutScene() ;
			
			if( !_isReverse )
				saveMoment();
						
			for( unsigned t=0 ; t<_tags.size() ; t++)
			{			
				
				bool isDone = false;
				
				if( _tags[t]->isValid( fDash + diffFrmCenter , _isReverse) )
				{	
					
					if( !isCutScene )
					{
						if( _tags[t]->isInitPos(fDash + diffFrmCenter) )
							_tags[t]->generateCutout( _frames[ Constants::FRAME_VOLUME / 2] );
						
						_tags[t]->addPos( _tracker.track( _frames , _tags[t]->getPos() , _isReverse , isDone ) );						
						
						if( isDone ) _tags[t]->setDone();						
					}
					else
						_tags[t]->setDone();
				}
											
			}//for t			
			
			
		}//if prep'd
		
	}//for f	
	
	LOG(LOG_MESSAGE,"Done tracking tags");

}


bool Video::testCutScene()
{
	return _frames[ Constants::FRAME_VOLUME / 2]->getClusterIndex() != _frames[ Constants::FRAME_VOLUME / 2 + 1]->getClusterIndex();
}

void Video::saveMoment()
{
	
	if( !_isProcessed )
	{
		Frame* f =  _frames[ Constants::FRAME_VOLUME / 2];
		
		_currentClusterCount[ f->getClusterIndex() ]++;
	
		if( _currentClusterCount[ f->getClusterIndex() ] == min( (unsigned)5, _clusterCount[ f->getClusterIndex() ]) )
		{
			LOG(LOG_MESSAGE,"Saving moment");
			
			IplImage* moment = f->generateThumbnail();
			cvSaveImage(momentFilename(f->getIndex() ).c_str() , moment);
			cvReleaseImage(&moment);
		}
	}// if ! processed

		
}


void Video::write()
{
	for( unsigned t=0 ; t<_tags.size() ; t++)
		_tags[t]->write(_width , _height);

	writeClusterIndex();
		
}


void Video::writeClusterIndex()
{

	if( !_isProcessed )
	{

		LOG( LOG_VERBOSE , "Writing video cluster index");
	
		ofstream out(_cachePath.c_str() , ios::out);
	
		out<<"<video>\n";
	
		for( unsigned c=0 ; c<_clusterIndex.size() ; c++)
			out<<"<f c=\"" + Utilities::toString(_clusterIndex[c]) + "\" />\n";
	
		out<<"</video>";	
		out.close();
	
	
	}
			

}


void Video::readClusterIndex()
{
	LOG( LOG_VERBOSE , "Reading video cluster index");
	
	XMLNode clusterIndex = XMLNode::parseFile(_cachePath.c_str(), "video");	
	
	for( unsigned int f=0 ; f<_totalFrames ; f++)
	{
		XMLNode frame = clusterIndex.getChildNode("f", f);
		_clusterIndex[f] = atoi(frame.getAttribute("c"));
		_clusterCount[ _clusterIndex[f] ]++;
	}
	
}


void Video::initFrames()
{
	LOG(LOG_VERBOSE,"Init Frames");
	
   for( unsigned f=0 ; f<Constants::FRAME_VOLUME ; f++)
    	_frames.push_back( new Frame() );
}


void Video::freeFrames()
{
	LOG(LOG_VERBOSE,"Free frames");

    for( unsigned f=0 ; f<_frames.size() ; f++)    
    {
        _frames[f]->free();
        delete _frames[f];
    }
    
    _frames.clear();
}

void Video::freeTags()
{
	LOG(LOG_VERBOSE,"Free tags");

   for( unsigned t=0 ; t<_tags.size() ; t++)
    {
        _tags[t]->free();
        delete _tags[t];
    }
    
         _tags.clear();
}

void Video::free()
{

	LOG(LOG_MESSAGE,"Free video");

	freeFrames();
	freeTags();

    cvReleaseCapture(&_stream);
    
   	LOG(LOG_MESSAGE,"Done free video");

    cvReleaseCapture(&_stream);
}

