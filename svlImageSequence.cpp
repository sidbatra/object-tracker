/*****************************************************************************
** STAIR VISION LIBRARY
** Copyright (c) 2007-2009, Stephen Gould
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Stanford University nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************
** FILENAME:    svlImageSequence.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
**              Olga Russakovsky <olga@cs.stanford.edu>
**
*****************************************************************************/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#if defined(_WIN32)||defined(WIN32)||defined(__WIN32__)
#include "win32/dirent.h"
#else
#include <dirent.h>
#endif

#include "xmlParser.h"

//#include "svlBase.h"
#include "svlImageSequence.h"

// svlImageSequence class --------------------------------------------------

svlImageSequence::svlImageSequence() : 
    _directoryName("."), _imageBuffer(NULL)
{
    // do nothing
}

svlImageSequence::svlImageSequence(const svlImageSequence& s) :
  _directoryName(s._directoryName), _imageNames(s._imageNames), _imageBuffer(NULL)
{
    // do nothing
}

svlImageSequence::~svlImageSequence()
{
    if (_imageBuffer != NULL)
	cvReleaseImage(&_imageBuffer);
}

void svlImageSequence::dir(const char *directory, const char *extension)
{
    //SVL_ASSERT(directory != NULL);

    clear();

    DIR *dir = opendir(directory);
    //SVL_ASSERT(dir != NULL);

    _directoryName = string(directory) + string("/");
    struct dirent *e = readdir(dir);
    while (e != NULL) {	
        if (strstr(e->d_name, extension) != NULL) {
            _imageNames.push_back(string(e->d_name));
        }
        e = readdir(dir);
    }

    // sort image names, so that sequence corresponds to frame index
    sort(_imageNames.begin(), _imageNames.end());

    closedir(dir);
}

void svlImageSequence::load(const char *filename)
{
    //SVL_ASSERT(filename != NULL);

    XMLNode root = XMLNode::parseFile(filename, "ImageSequence");
    if (root.isEmpty()) {
       // SVL_LOG(SVL_LOG_WARNING, "image sequence " << filename << " is empty");
        return;
    }

    clear();

    if (root.getAttribute("dir") != NULL) {
      _directoryName = string(root.getAttribute("dir"));
      if (_directoryName[_directoryName.length()-1] != '/')
	_directoryName += string("/");
    } else {
        _directoryName = "";
    }

    bool warn = false;
    for (int i = 0; i < root.nChildNode("Image"); i++) {
	XMLNode node = root.getChildNode("Image", i);
	string name = node.getAttribute("name");
	_imageNames.push_back(name);

	if (node.nChildNode("region") > 0) 
	  warn = true;
    }
    //if (warn)
    //  SVL_LOG(SVL_LOG_WARNING, "regions not suppported within this application");
}

void svlImageSequence::save(const char *filename) const
{
  save(filename, -1);
}

void svlImageSequence::save(const char *filename, const int numFileToSave) const
{
    //SVL_ASSERT(filename != NULL);
    ofstream ofs(filename);
    ofs << "<ImageSequence\n"
        << "  dir=\"" << _directoryName << "\"\n"
        << "  version=\"1.0\">\n";
    unsigned totalFile = 0;
    if (numFileToSave > (int)_imageNames.size() || numFileToSave < 0)
      totalFile = _imageNames.size();
    else
      totalFile = numFileToSave;

    for (unsigned i = 0; i < totalFile; i++)
      ofs << "  <Image name=\"" << _imageNames[i] << "\"/>\n";
   
    ofs << "</ImageSequence>\n";
    ofs.close();
}

void svlImageSequence::shuffleImages(unsigned groupSize)
{
  if (_imageNames.size() % groupSize != 0) {
    //SVL_LOG(SVL_LOG_WARNING, "Can't shuffle an image sequence in groups of "
	 //   << groupSize << " when there are " << _imageNames.size()
	 //   << " images total");
    return;
  }
 
  vector<int> perm(_imageNames.size()/groupSize);
  for (unsigned i = 0; i < perm.size(); i++)
    perm[i] = i*groupSize;
  
  vector<string> names = _imageNames;
  for (unsigned i = 0; i < _imageNames.size(); i++) 
    _imageNames[i] = names[perm[i/groupSize] + i%groupSize];
}

void svlImageSequence::add(const svlImageSequence &s)
{
  //if (strWithoutEndSlashes(s._directoryName) != strWithoutEndSlashes(_directoryName)) {
    //SVL_LOG(SVL_LOG_WARNING, "Can't merge image sequences corresponding to different directories");
  //  return;
  //}

  // not very efficient
  vector<string>::const_iterator first = _imageNames.begin();
  vector<string>::const_iterator last = _imageNames.end();
  for (unsigned i = 0; i < s._imageNames.size(); i++) {
    string name = s._imageNames[i];
    if (find(first, last, name) == last)
      _imageNames.push_back(name);
  }
}

void svlImageSequence::subtract(const svlImageSequence &s)
{
  //if (strWithoutEndSlashes(s._directoryName) != strWithoutEndSlashes(_directoryName)) {
    //SVL_LOG(SVL_LOG_WARNING, "Can't subtract image sequences corresponding to different directories");
  //  return;
  //}

  // not very efficient
  for (unsigned i = 0; i < s._imageNames.size(); i++) {
    string name = s._imageNames[i];
    vector<string>::iterator it = find(_imageNames.begin(), _imageNames.end(), name);
    if (it != _imageNames.end())
      _imageNames.erase(it);
  }
}

const IplImage *svlImageSequence::image(unsigned index)
{
    //SVL_ASSERT(index < _imageNames.size());
    if (_imageBuffer != NULL)
	cvReleaseImage(&_imageBuffer);
    string filename = _directoryName + _imageNames[index];
    _imageBuffer = cvLoadImage(filename.c_str());

    if (!_imageBuffer)
      {
	//SVL_LOG(SVL_LOG_WARNING, "svlImageSequence::image: Unabled to load "<<filename);
      }

#if defined(_WIN32)||defined(WIN32)||defined(__WIN32__)
    cvFlip(_imageBuffer, NULL, 0);
#endif

#if 0
    cvNamedWindow("debug", 1);
    cvShowImage("debug", _imageBuffer);
    cvWaitKey(-1);
    cvDestroyWindow("debug");
#endif

    return _imageBuffer;
}

void svlImageSequence::setDirectory(string directoryName)
{
  int len = directoryName.length();
  if (len > 0 && directoryName[len-1] != '/')
    directoryName.push_back('/');

  _directoryName = directoryName;
}

bool hasHomogeneousExtensions(const svlImageSequence & s)
{
  if (s.empty())
    {
      return false;
    }

  string ext = getExtension(s);

  for (unsigned int i = 1; i < s.numImages(); i++)
    {
      string name = s[i];
      size_t lastDot = name.find_last_of(".");
      if (lastDot == string::npos)
	return false;

      string thisExt = name.substr(lastDot);

      if ( strcmp(thisExt.c_str(), ext.c_str()) != 0)
	{
	  return false;
	}
    }

  return true;
}

string getExtension(const svlImageSequence & s)
{
  //SVL_ASSERT(! s.empty());

  string first = s[0];
  
  size_t lastDot = first.find_last_of(".");
  
  //SVL_ASSERT(lastDot != string::npos);

  return first.substr(lastDot);
}

