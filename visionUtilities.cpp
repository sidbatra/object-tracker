/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    visionUtilities.cpp
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Definitions for the functions in visionUtilities.h
*****************************************************************************/

#include "visionUtilities.h"

//Computes the vectors of means and variances of the given image
void VisionUtilities::computeImageMeansVariances(IplImage *image, vector<double> &means , vector<double> &variances)
{
    //Clean & initialize vectors
    means = vector<double>(3);
    variances = vector<double>(3);

    int totalPixels = image->height * image->width;
  
    //Set to initial values
    for( int i=0 ; i<3 ; i++)
        means[i] = variances[i] = 0;

    cvCvtColor(image , image , CV_RGB2HSV); //Change to HSV color model //Try BGR just for testing

    //Compute means of the given image
    for( int y=0 ; y<image->height ; y++)
    {
        for( int x=0 ; x<image->width ; x++)
        {
            CvScalar pixelColor = VisionUtilities::getRGB(image,x,y);

            for( int i=0 ; i<3 ; i++)
                means[i] += pixelColor.val[i];
        }
    }

    for( int i=0 ; i<3 ; i++)
        means[i] /= totalPixels;

    //Compute variances of the given image
    for( int y=0 ; y<image->height ; y++)
    {
        for( int x=0 ; x<image->width ; x++)
        {
            CvScalar pixelColor = VisionUtilities::getRGB(image,x,y);

            for( int i=0 ; i<3 ; i++)
                variances[i] += pow( means[i] -  pixelColor.val[i] , 2);
        }
    }

    for( int i=0 ; i<3 ; i++)
        variances[i] /= totalPixels;


    cvCvtColor(image , image , CV_HSV2RGB); //Convert back to RGB color model
}

//Reads a transparent PNG in the form of a text file and populates an image and a mask from it
void VisionUtilities::readTransparentPNG(string filename , IplImage **content , IplImage **mask)
{

    ifstream ifs(filename.c_str());

    //Used to read values in the file
    int r,g,b,a;

    int width =0 , height = 0;
    ifs>>width>>height;	


    //Create IplImage to hold matrix
    *content = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
    *mask  = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	

    for (int y = 0; y <height ; y++)
       for (int x = 0 ;  x < width ; x++)
        {
            ifs>>r>>g>>b>>a;
	
            cvSet2D(*content,y,x,cvScalar(b,g,r));
	    CV_IMAGE_ELEM(*mask, unsigned char,y,x) = a;
        }


   ifs.close();

}


//Converts the given image to grayscale
IplImage *VisionUtilities::toGrayscale(IplImage *source)
{
        IplImage *gray = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
        cvCvtColor(source,gray,CV_RGB2GRAY);
        return gray;
}

//Converts the given image to a 10*10 tiny image
IplImage *VisionUtilities::computeSpatialImage(IplImage *source)
{
        IplImage *tiny = cvCreateImage(Constants::SPATIAL_IMAGE_SIZE,IPL_DEPTH_8U,3);
        cvResize(source,tiny,CV_INTER_NN);

        return tiny;
}


//Computes the l1-norm of the two spatial images
int VisionUtilities::compareSpatialImages(IplImage *one , IplImage *two)
{
        int diff = 0;

        for( int y=0 ; y<Constants::SPATIAL_IMAGE_SIZE.height ; y++ )
                for( int x=0 ; x<Constants::SPATIAL_IMAGE_SIZE.width ; x++ )
                {
                        diff += abs( CV_IMAGE_ELEM(one,unsigned char,y,x) - CV_IMAGE_ELEM(two,unsigned char,y,x) );

                }

        return diff;
}

//Computes the histogram of the give image
vector<float> VisionUtilities::computeHistogram(IplImage *source , bool freeSource)
{

    vector<float> featureVector;

    //Create objects to hold each plane of the color model
    IplImage* lab    = cvCreateImage( cvGetSize(source), 8, 3 );
    IplImage* lPlane = cvCreateImage( cvGetSize(source), 8, 1 );
    IplImage* aPlane = cvCreateImage( cvGetSize(source), 8, 1 );
    IplImage* bPlane = cvCreateImage( cvGetSize(source), 8, 1 );

    int l_bins = 10, a_bins = 32 , b_bins = 32;
    int l_size[] = {l_bins}; int a_size[] = {a_bins}; int b_size[] = {b_bins};
    float l[]={0,256} ; float a[]={0,255} ; float b[]={0,255};
    float *l_ranges[] = { l };  float *a_ranges[] = {a};  float *b_ranges[] = {b};

    CvHistogram* lhist = cvCreateHist( 1, l_size, CV_HIST_ARRAY, l_ranges, 1 );
    CvHistogram* ahist = cvCreateHist( 1, a_size, CV_HIST_ARRAY, a_ranges, 1 );
    CvHistogram* bhist = cvCreateHist( 1, b_size, CV_HIST_ARRAY, b_ranges, 1 );

    cvCvtColor( source, lab, CV_BGR2Lab );
    cvSplit(lab,lPlane,aPlane,bPlane,NULL);

    cvCalcHist(&lPlane,lhist,0,NULL);
    cvCalcHist(&aPlane,ahist,0,NULL);
    cvCalcHist(&bPlane,bhist,0,NULL);

    for( int i=0 ; i<l_bins ; i++)
        featureVector.push_back(cvQueryHistValue_1D(lhist,i));
    for( int i=0 ; i<a_bins ; i++)
        featureVector.push_back(cvQueryHistValue_1D(ahist,i));
    for( int i=0 ; i<b_bins ; i++)
        featureVector.push_back(cvQueryHistValue_1D(bhist,i));


    cvReleaseImage(&lab);
    cvReleaseImage(&lPlane);
    cvReleaseImage(&aPlane);
    cvReleaseImage(&bPlane);
    cvReleaseHist(&lhist);
    cvReleaseHist(&ahist);
    cvReleaseHist(&bhist);

    if( freeSource )
        cvReleaseImage(&source);

    return featureVector;

}

//Compaes two hsitograms based on the chosen method
double VisionUtilities::compareHistograms(CvHistogram *histOne , CvHistogram *histTwo)
{
    return cvCompareHist( histOne , histTwo , CV_COMP_CHISQR);
}




//Checks whether the given frame is almost blank or not
bool VisionUtilities::isBlankFrame( IplImage *frame )
{
    bool isBlank = false;
    IplImage *temp = cvCreateImage(cvGetSize(frame),frame->depth,1);
    cvCvtColor(frame,temp,CV_RGB2GRAY);   //Convert image to grayscale

    double mean = 0.0;

    //Iterate over each pixel to compute the mean of the grayscale image
    for( int y=0 ; y<temp->height ; y++)
        for( int x=0 ; x<temp->width ; x++)
            mean += CV_IMAGE_ELEM(temp,unsigned char,y,x);

    mean /= temp->height * temp->width;

    if( mean < Constants::THRESHOLD_FOR_BLANK_FRAME )
        isBlank = true;

    //Free memory
    cvReleaseImage(&temp);

    return isBlank;
}

//Scales the given polygon
void VisionUtilities::scalePolygon( vector<Point> &polygon , double scale)
{
    for( unsigned i=0 ; i<polygon.size() ; i++)
    {
        polygon[i].x *= scale;
        polygon[i].y *= scale;
    }
}


//Generates the poisson edge map for the given image
IplImage* VisionUtilities::poissonEdgeMap(IplImage *image)
{

    //Populate the kernel needed to generate the edge map
    CvMat *kernel = cvCreateMat(3,3,CV_32F);
	
    double ker[9] = {1 ,0 ,-1, 2 ,0 ,-2, 1 ,0 ,-1}; 

	for( int i=0 ; i<3  ; i++)
		for( int j=0 ; j<3 ; j++)
			cvmSet(kernel,i,j,ker[i*3+j]);
    
    //Create IplImage object to hold the final result
    IplImage *tempI   = cvCreateImage(cvSize(image->width+2,image->height+2),IPL_DEPTH_32F,image->nChannels);
    IplImage *edges   = cvCreateImage(cvSize(image->width+2,image->height+2),IPL_DEPTH_32F,image->nChannels);
    IplImage *final   = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_32F,image->nChannels);

    //Create a temp image with float type, that has padded zeros on all sides
    for( int y=0 ; y<tempI->height ; y++)
	{          
		for( int x=0; x<tempI->width ; x++)
		{    
            if( x!=0 && x!=tempI->width-1 && y!=0 && y!=tempI->height-1 )
            {
                CV_IMAGE_ELEM(tempI,float,y,x) = (float)CV_IMAGE_ELEM(image,unsigned char,y,x);
            }
            else
            {                
                CV_IMAGE_ELEM(tempI,float,y,x) = 0.0;
            }
        }
    }

    //Compute poisson style edge map
	cvFilter2D(tempI,edges,kernel);

    
    for( int y=2 , finalY = 0 ; finalY<final->height ; y++ , finalY++)
	{
        float alpha = 0;
        
        for( int x=2 , finalX = 0 ; finalX< final->width  ; x++ , finalX++)
		{
            if( finalY == 0 || finalY >= final->height-3 || finalX == 0 || finalX == final->width-1)
            {
                CV_IMAGE_ELEM(final,float,finalY,finalX) = 255 ;

            }
            else
            {
                CV_IMAGE_ELEM(final,float,finalY,finalX) = 255 - alpha;
            
                alpha += CV_IMAGE_ELEM(edges,float,y,x);            
            }
        }
        
    }

    //Free memory
    cvReleaseMat(&kernel);
    cvReleaseImage(&edges);
    cvReleaseImage(&tempI);
      
	return final;
}


//Computes subpixel values in the image
double VisionUtilities::subPixel( IplImage *I , double &PiX , double &PiY)
{
    int intPx = (int)PiX;
    int intPy = (int)PiY;
	
	double x1 = CV_IMAGE_ELEM(I,unsigned char,intPy,intPx) 
		+  (PiX - intPx) * ( CV_IMAGE_ELEM(I,unsigned char,intPy ,intPx + 1) - CV_IMAGE_ELEM(I,unsigned char,intPy,intPx) );

	double x2 = CV_IMAGE_ELEM(I,unsigned char,intPy+1 ,intPx) 
		+ (PiX - intPx) * ( CV_IMAGE_ELEM(I,unsigned char,intPy+1 ,intPx + 1) - CV_IMAGE_ELEM(I,unsigned char,intPy+1,intPx) );

	return x1 + (PiY - intPy) * (x2 - x1);
}

//Computes subpixel values in the image
double VisionUtilities::subPixelFloat( IplImage *I , double &PiX , double &PiY)
{
    int intPx = (int)PiX;
    int intPy = (int)PiY;
	
	double x1 = CV_IMAGE_ELEM(I,float,intPy,intPx) 
		+  (PiX - intPx) * ( CV_IMAGE_ELEM(I,float,intPy ,intPx + 1) - CV_IMAGE_ELEM(I,float,intPy,intPx) );

	double x2 = CV_IMAGE_ELEM(I,float,intPy+1 ,intPx) 
		+ (PiX - intPx) * ( CV_IMAGE_ELEM(I,float,intPy+1 ,intPx + 1) - CV_IMAGE_ELEM(I,float,intPy+1,intPx) );

	return x1 + (PiY - intPy) * (x2 - x1);
}


//Computes subpixel values in the RGB image
CvScalar VisionUtilities::subPixelRGB( IplImage *I , double &PiX , double &PiY)
{
   
    int intPx = (int)PiX;
    int intPy = (int)PiY;
    CvScalar result;

    uchar* left = &((uchar*)(I->imageData + I->widthStep*(intPy)))[(intPx)*3];
    uchar* right = &((uchar*)(I->imageData + I->widthStep*(intPy)))[(intPx+1)*3];
    uchar* bottomLeft = &((uchar*)(I->imageData + I->widthStep*(intPy+1)))[(intPx)*3];
    uchar* bottomRight = &((uchar*)(I->imageData + I->widthStep*(intPy+1)))[(intPx+1)*3];

	//Floating point of the coordinates
	double delX = PiX - intPx;
	double delY = PiY - intPy;

    double x1 = left[0] +  delX * ( right[0] - left[0] );
    double x2 = bottomLeft[0] + delX * ( bottomRight[0] - bottomLeft[0] );
    result.val[0] = x1 + delY * (x2 - x1);

    x1 = left[1] +  delX * ( right[1] - left[1] );
    x2 = bottomLeft[1] + delX * ( bottomRight[1] - bottomLeft[1] );
    result.val[1] = x1 + delY * (x2 - x1);
 
    x1 = left[2] +  delX * ( right[2] - left[2] );
    x2 = bottomLeft[2] + delX * ( bottomRight[2] - bottomLeft[2] );
    result.val[2] = x1 + delY * (x2 - x1);

	return result;
}

//Forms a polygon from the given rectangle
vector<Point> VisionUtilities::formPolygonFromRect(CvRect rect)
{
    vector<Point> polygon(4);
    polygon[0].x = rect.x; polygon[0].y = rect.y;
    polygon[1].x = rect.x + rect.width -1; polygon[1].y = rect.y;
    polygon[2].x = rect.x + rect.width-1; polygon[2].y = rect.y + rect.height-1;
    polygon[3].x = rect.x; polygon[3].y = rect.y + rect.height-1;

    return polygon;
}


//Forms a polygon from the corners of the image
vector<Point> VisionUtilities::formPolygonFromImage(IplImage *image)
{
    vector<Point> polygon(4);
    polygon[0].x = 0; polygon[0].y = 0;
    polygon[1].x = image->width-2; polygon[1].y = 0;
    polygon[2].x = image->width-2; polygon[2].y = image->height-2;
    polygon[3].x = 0; polygon[3].y = image->height-2;

    return polygon;
}

CvMat* VisionUtilities::findMappingBetweenPolygons(vector<Point> sourcePolygon , vector<Point> targetPolygon , int targetOffsetX , int targetOffsetY)
{
    //Create matrices to hold correspondences
    vector<CvMat*> matches(2);
    
    matches[0] = cvCreateMat( 2, (int)sourcePolygon.size(), CV_64FC1);
    matches[1] = cvCreateMat( 2, (int)targetPolygon.size(), CV_64FC1);

    //Populate correspondences
    for( unsigned i=0 ; i<sourcePolygon.size() ; i++)
    {
        CV_MAT_ELEM(*(matches[0]),double,0,i) = sourcePolygon[i].x;
        CV_MAT_ELEM(*(matches[0]),double,1,i) = sourcePolygon[i].y;

        CV_MAT_ELEM(*(matches[1]),double,0,i) = targetPolygon[i].x - targetOffsetX;
        CV_MAT_ELEM(*(matches[1]),double,1,i) = targetPolygon[i].y - targetOffsetY;
    }
    
    CvMat *H = VisionUtilities::estimateHomography(matches);

    cvReleaseMat(&matches[0]);
    cvReleaseMat(&matches[1]);

    return H;
	
}

//Computes a pseduo inverse of the correspondences to estimate the affine transformation matrix
CvMat* VisionUtilities::estimateHomography(vector<CvMat*> matches)
{
    CvMat* A = cvCreateMat(3,3,CV_64FC1);
    cvFindHomography(matches[0] , matches[1] , A);
    return A;
}

CvMat* VisionUtilities::estimateHomography(vector<vector<Point> > matches , bool makePlanar)
{
	unsigned int totalSamples = matches.size();	
	CvMat* A = cvCreateMat(3,3,CV_64FC1);
	
	CvMat* prev = cvCreateMat( 2, totalSamples , CV_64FC1);
    CvMat* next = cvCreateMat( 2, totalSamples , CV_64FC1);

    for( unsigned i=0 ; i<totalSamples ; i++ )
    {
        CV_MAT_ELEM(*prev ,double, 0,i) = matches[i][0].x;
        CV_MAT_ELEM(*prev ,double, 1,i) = matches[i][0].y;
 
        CV_MAT_ELEM(*next ,double, 0,i) = matches[i][1].x;
        CV_MAT_ELEM(*next ,double, 1,i) = matches[i][1].y;
    }

    cvFindHomography(prev , next , A);
    
    
    if( makePlanar )
    {
    	CV_MAT_ELEM(*A ,double, 0,1) = 0.0;
     	CV_MAT_ELEM(*A ,double, 1,0) = 0.0;
     	CV_MAT_ELEM(*A ,double, 2,0) = 0.0;
     	CV_MAT_ELEM(*A ,double, 2,1) = 0.0;
     	CV_MAT_ELEM(*A ,double, 2,2) = 1.0;
	}
	
	cvReleaseMat(&prev);
	cvReleaseMat(&next);
        
    return A;	
}


//Finds the possibly larger enclosing rectangle for the given polygon
void VisionUtilities::findEnclosingRectangle(vector<Point> polygon,double &minX,double &minY ,double &maxX ,double &maxY)
{
	//Initial values
	minX = polygon[0].x;
	maxX = polygon[0].x;
	minY = polygon[0].y;
	maxY = polygon[0].y;

	for( unsigned i=1 ; i<polygon.size() ; i++)
	{
		if( polygon[i].x < minX )
			minX = polygon[i].x;
		if( polygon[i].x > maxX )
			maxX = polygon[i].x;
		if( polygon[i].y < minY )
			minY = polygon[i].y;
		if( polygon[i].y > maxY )
			maxY = polygon[i].y;
	}

}

//Finds the centre of the given quadrilaterlal
Point VisionUtilities::findCentreOfRegion( vector<Point> region )
{
    double maxX , maxY , minX , minY;

    VisionUtilities::findEnclosingRectangle(region,minX,minY,maxX,maxY);

    return Point((maxX+minX)/2,(maxY+minY)/2);
    
}

//Tests if the two given frames are the same
int VisionUtilities::isRepeatFrame(IplImage *one , IplImage *two)
{
    //Init to default return value
    int isRepeat = 0;

    double l1 = 0.0;

    //Compute l1 norm of the difference between the rgb vectors of each pixel in the two images
    for( int y=0 ; y<one->height ; y++)
        for( int x=0 ; x<one->width ; x++)
        {
            CvScalar oneVector = VisionUtilities::getRGB(one,x,y);
            CvScalar twoVector = VisionUtilities::getRGB(two,x,y);

            l1 += fabs( oneVector.val[0] - twoVector.val[0]) + 
                  fabs( oneVector.val[1] - twoVector.val[1]) + 
                  fabs( oneVector.val[2] - twoVector.val[2]) ;
        }
       
     //Compute average l1 norm of the pixels in the images
     l1 /= one->height * one->width;

     cout<<" "<<l1;

     //if average l1 norm is below threshold
     if( l1 < Constants::THRESHOLD_FOR_REPEAT_FRAME )
         isRepeat = 1;

    return isRepeat;
}

//Computes the shortest distance from the given point to one of polygon edges
double VisionUtilities::shortestDistanceToEdge(vector<Point> &polygon, double &pX , double &pY)
{	
	Point p1 = polygon[0] , p2;
	double minimumDistance = 9999999;

	for (unsigned i=1 ; i<=polygon.size() ; i++ )
	{
		p2 = polygon[i % polygon.size()];

		double distance = fabs( (p2.x - p1.x) * (p1.y-pY) - (p1.x - pX) * (p2.y - p1.y));
		distance /= sqrt( pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));

		if( distance < minimumDistance )
			minimumDistance = distance;

		p1 = p2;
	}

	return minimumDistance;
}

//Checks if the given point lies within the given polygon or not
bool VisionUtilities::inPolygon(vector<Point> &polygon, double &pX , double &pY)
{
	//Total number of horizontal intercepts made with polygon
    int totalIntercepts = 0;
	bool isInside = 0;

	Point p1 = polygon[0] , p2;

	for (unsigned i=1 ; i<=polygon.size() ; i++ )
	{
		
		p2 = polygon[i % polygon.size()];

		if (pY > MIN(p1.y,p2.y)) 
		{
			  if (pY <= MAX(p1.y,p2.y)) 
			  {
				if (pX <= MAX(p1.x,p2.x)) 
				{
					if (p1.y != p2.y) 
					{
						double xinters = (pY-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || pX <= xinters)
						totalIntercepts++;
					}
				}
		}
		}

		p1 = p2;
	}


	//If intercepts are not even
	if( totalIntercepts % 2 == 1 )
		isInside = 1;

      return isInside;
}



//Fast method of retriving the values of a color image
CvScalar VisionUtilities::getRGB( IplImage *image , int &x , int &y )
{
    uchar* temp_ptr = &((uchar*)(image->imageData + image->widthStep* y ))[x*3];
    return cvScalar(temp_ptr[0],temp_ptr[1],temp_ptr[2]);  
}

////Fast method of setting the values of a color image
//void VisionUtilities::setRGB( IplImage *image , int &x , int &y , CvScalar newValue)
//{
//    uchar* temp_ptr = &((uchar*)(image->imageData + image->widthStep* y ))[x*3];
//    temp_ptr[0] = (uchar)newValue.val[0]; 
//    temp_ptr[1] = (uchar)newValue.val[1];
//    temp_ptr[2] = (uchar)newValue.val[2];    
//}


//Computes the connected components in an image and removes the ones below a certain size 
void VisionUtilities::removeSmallComponents( IplImage *occlusionModel , double areaThershold, bool invert , bool keepEdgeComponents)
{
            
        //Create structures need to store connected components in the image
        IplImage* colorMap = cvCreateImage( cvGetSize(occlusionModel), IPL_DEPTH_8U, 3 );
        CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* contour = 0;
        int maskArea = occlusionModel->width * occlusionModel->height;


        
        //If the image is to be inverted
        if( invert )
        {
            //Invert every value in the binary image
            for( int y=0 ; y<occlusionModel->height ; y++)
                for( int x=0 ; x<occlusionModel->width ; x++)
                    CV_IMAGE_ELEM(occlusionModel,unsigned char,y,x) = 255 - CV_IMAGE_ELEM(occlusionModel,unsigned char,y,x);
        }
        
        //Compute the contours
        IplImage *temp = cvCloneImage(occlusionModel);
        cvFindContours( temp, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        cvReleaseImage(&temp);
        
        cvZero( colorMap );
        
        for( ; contour != 0; contour = contour->h_next )
        {
            //Generate random color for contour
            CvScalar color = CV_RGB( rand() % 255, rand() % 255, rand() % 255); 
            
            //Draw contour with the randomly generated color
            cvDrawContours( colorMap, contour, color, color, -1, CV_FILLED, 8 );
            
            CvRect r = cvContourBoundingRect(contour);

            if( fabs(cvContourArea(contour)) / (maskArea+0.0) < areaThershold &&
                ( !keepEdgeComponents || (  r.x > 1 && r.y > 1 && r.x+r.width < occlusionModel->width-1 
                                            && r.y+r.height < occlusionModel->height-1 )) )
                
            {
                //Wipe out the component from the mask
                for( int y=r.y ; y<r.y+r.height ; y++)  
                    for( int x=r.x ; x<r.x+r.width; x++)
                    {
                        CvScalar c = VisionUtilities::getRGB(colorMap,x,y);

                        if( c.val[0] == color.val[0] && c.val[1] == color.val[1] && c.val[2] == color.val[2] )
                            CV_IMAGE_ELEM(occlusionModel,unsigned char,y,x) = 0;                        
                    }
            }
            
        }//for contour

        //utilities::display(colorMap , "Components");

        //If the image is to be inverted
        if( invert )
        {
            //Invert every value in the binary image
            for( int y=0 ; y<occlusionModel->height ; y++)
                for( int x=0 ; x<occlusionModel->width ; x++)
                    CV_IMAGE_ELEM(occlusionModel,unsigned char,y,x) = 255 - CV_IMAGE_ELEM(occlusionModel,unsigned char,y,x);
        }

        //Free memory
        cvReleaseImage(&colorMap);

        if( storage != NULL )
            cvReleaseMemStorage(&storage);
        
}




//Checks whether the given polygon lies within the image or not
int VisionUtilities::checkOutofBounds(IplImage *image , vector<Point> polygon , int margin)
{
    int isOutside = 1;

    for( unsigned i=0 ; i<polygon.size() ; i++)
    {        
        if( !(polygon[i].x - margin < 0 || polygon[i].x + margin - image->width  > 0 ||
                    polygon[i].y - margin < 0 || polygon[i].y + margin - image->height  > 0) )
        {
            isOutside = 0;
            break;
        }
    }

    return isOutside;
}


//Checks whether the project of the given polygon is out of bounds
int VisionUtilities::checkOutofBounds(IplImage *image , vector<Point> polygon , CvMat *H , int margin)
{
    int isOutside = 1;

    for( int i=0 ; i<(int)polygon.size() ; i++)
    {
        Point p = polygon[i];
        VisionUtilities::projectCoordinate(H,p.x,p.y);

        if( !(p.x - margin  < 0 || p.x + margin - image->width  > 0 ||
                    p.y - margin < 0 || p.y + margin - image->height  > 0) )
        {
            isOutside = 0;
            break;
        }
    }

    return isOutside;
}

//Performs the opeation H * X .. where H is the homography matrix and X = [ x y z ] the coordinate vector
//where z is assumed to be 1 and the nprojects the new x y coordinates via xNew = xNew / zNew &  yNew = yNew / zNew
void VisionUtilities::projectCoordinate(CvMat *homography , double &x , double &y)
{
	double xOld = x;
	double yOld = y;
	double zOld = 1.0;
	double z = 0.0;
	

	x = xOld * CV_MAT_ELEM(*homography,double,0,0) + yOld * CV_MAT_ELEM(*homography,double,0,1) + zOld * CV_MAT_ELEM(*homography,double,0,2);
	y = xOld * CV_MAT_ELEM(*homography,double,1,0) + yOld * CV_MAT_ELEM(*homography,double,1,1) + zOld * CV_MAT_ELEM(*homography,double,1,2);
	z = xOld * CV_MAT_ELEM(*homography,double,2,0) + yOld * CV_MAT_ELEM(*homography,double,2,1) + zOld * CV_MAT_ELEM(*homography,double,2,2);

	(x) = (x) / (z);
	(y) = (y) / (z);
	
}


////Estimates based on histograms whether the current 
//int VisionUtilities::estimateCutScene(IplImage *imageOne, IplImage *imageTwo)
//{   
//     //Clone the images for blurring
//    IplImage *one = cvCloneImage(imageOne);    
//    IplImage *two = cvCloneImage(imageTwo);
//    IplImage *planeOne[] = {one};
//    IplImage *planeTwo[] = {two};
//    int size[] = {256};
//
//    cvSmooth(one,one,CV_GAUSSIAN,Constants::SMOOTHING_MASK_SIZE,Constants::SMOOTHING_MASK_SIZE);
//    cvSmooth(two,two,CV_GAUSSIAN,Constants::SMOOTHING_MASK_SIZE,Constants::SMOOTHING_MASK_SIZE);
//
//    //Init default return value
//    int isCutScene = 0;
//
//    CvHistogram *histOne = cvCreateHist(1,size,CV_HIST_ARRAY);
//    CvHistogram *histTwo = cvCreateHist(1,size,CV_HIST_ARRAY);
//    cvCalcHist(planeOne,histOne);
//    cvCalcHist(planeTwo,histTwo);
//
//    cvNormalizeHist(histOne,1);
//    cvNormalizeHist(histTwo,1);
//    double value = cvCompareHist(histOne,histTwo,CV_COMP_CHISQR );
//
//    cout<<"\n diff - "<<value;
//
//    
//    if( value  > Constants::CUT_SCENE_THRESHOLD )
//        isCutScene = 1;
//
//    //Free memory
//    cvReleaseImage(&one);
//    cvReleaseImage(&two);
//
//    return isCutScene;
//}



//Estimates whether the given scene makes a cut scene based upon
//image differencing
//int VisionUtilities::estimateCutScene(IplImage *imageOne, IplImage *imageTwo)
//{
//     //Clone the images for blurring
//    IplImage *one = cvCloneImage(imageOne);    
//    IplImage *two = cvCloneImage(imageTwo);
//    
//
//    cvSmooth(one,one,CV_GAUSSIAN,Constants::SMOOTHING_MASK_SIZE,Constants::SMOOTHING_MASK_SIZE);
//    cvSmooth(two,two,CV_GAUSSIAN,Constants::SMOOTHING_MASK_SIZE,Constants::SMOOTHING_MASK_SIZE);
//
//    //Init default return value
//    int isCutScene = 0;
//
//    double sum = 0.0;
//
//    //Compute the square difference sum between the pixel value
//    for( int y=0 ; y<one->height ; y++)
//    {
//        for( int x=0 ; x<one->width ; x++)
//        {
//            int temp = CV_IMAGE_ELEM(one,unsigned char,y,x) - CV_IMAGE_ELEM(two,unsigned char,y,x);
//            sum += temp * temp;
//
//        }
//    }
//
//    sum /= one->height * one->width;
//
//    cout<<"\n SUM - "<<sum;
//
//    //Test for cut scene
//    if( sum > Constants::CUT_SCENE_THRESHOLD )
//        isCutScene = 1;
//
//    //Free memory
//    cvReleaseImage(&one);
//    cvReleaseImage(&two);
//
//    return isCutScene;
//}

