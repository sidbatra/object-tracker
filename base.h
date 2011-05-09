/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    base.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Includes the core set of headers
*****************************************************************************/

#pragma once

using namespace std;

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)||defined(WIN32)||defined(__WIN32__)
#include <windows.h>
#include "dirent.h"
#else
#include <dirent.h>
#endif

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "point.h"

#include "constants.h"
#include "logger.h"
#include "utilities.h"
#include "visionUtilities.h"
#include "xmlParser.h"

