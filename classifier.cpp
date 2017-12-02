#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>

#include "cv.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

Rect findBiggestBlob(cv::Mat & matImage){
    int largest_area=0;
    
    vector< vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Rect bounding_rect;
    
    findContours( matImage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
    
    for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour.
        double a=contourArea( contours[i],false);  //  Find the area of contour
        if(a>largest_area){
            largest_area=a;
            bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }
    }
    
    return bounding_rect;
}

int main(int argc, char** argv)
{
    string fileName = "data/original_video/test.MOV";
    VideoCapture capture(fileName);
    
    
    Mat frame;
    int i;
    int j;
    
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    
    namedWindow( "w", 1);
    
    int t = 0;
    while(1)
    {
        capture >> frame;
        if(frame.empty())
            break;
        
        
        //SAVE DATA CODE-------------
        
        
        
        //save orig image - I(x,y,t)
        fileName ="data/I/"+to_string(t)+".png";
        imwrite( fileName,frame);
        
        
        
        //END SAVING DATA---------------
        //next t
        t++;
        
        waitKey(20); // waits to display frame
    }
    
    
    
    
    //----create ET(x,y,t), set T = 67
    fileName ="data/I/"+to_string(0)+".png";
    Mat firstImage = imread( fileName, 1 );
    
    fileName ="data/I/"+to_string(1)+".png";
    Mat secondImage = imread( fileName, 1 );
    
    Mat diffImage,foregroundMask;
    
    //turn E image to gray scale
    Mat EImage;
    
    
    //end
    
    
    //calc diffrence
    absdiff(secondImage, firstImage, diffImage);
    
    foregroundMask = Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);
    
    float threshold = 30.0f;
    float dist;
    
    for(int j=0; j<diffImage.rows; ++j){
        for(int i=0; i<diffImage.cols; ++i)
        {
            cv::Vec3b pix = diffImage.at<cv::Vec3b>(j,i);
            
            dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
            dist = sqrt(dist);
            
            if(dist>threshold)
            {
                foregroundMask.at<unsigned char>(j,i) = 4;
            }
        }
    }
    //end calc
    EImage =foregroundMask;
    
    for(i=2; i<t;i++) {
        int currentImageIndex = i;
        
        //if no such image
        if (currentImageIndex < 1)
            break;
        
        fileName ="data/I/"+to_string(currentImageIndex-1)+".png";
        firstImage = imread( fileName, 1 );
        
        fileName ="data/I/"+to_string(currentImageIndex)+".png";
        secondImage = imread( fileName, 1 );
        
        
        //calc diffrence
        Mat diffImage,foregroundMask;
        absdiff(secondImage, firstImage, diffImage);
        
        int historyIndex = (i)*4;
        
        if (historyIndex > 255)
            historyIndex = 255;
        
        for(int j=0; j<diffImage.rows; ++j)
            for(int i=0; i<diffImage.cols; ++i)
            {
                cv::Vec3b pix = diffImage.at<cv::Vec3b>(j,i);
                
                dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
                dist = sqrt(dist);
                
                if(dist>threshold)
                {
                    EImage.at<unsigned char>(j,i) = historyIndex;
                }
            }
        
    }
    //----end create ET(x,y,t), set T = 67
    
    printf("finished creating data\n");
    
    
    //now ETimage has the Etimage of the new video
    
    Rect EImageIntests=findBiggestBlob(EImage);
    Mat EImageCropped = EImage(EImageIntests);
    
    //test
    fileName ="data/MEI/1.png";
    imwrite( fileName,EImageCropped);
    //end test
    
    
    int numOfMEIPerDataOption = 3;
    int dataOptions = 3;
    string names[3] = {"crunch","hands","sit"};
    
    //get min comparison
    int minResult = EImageCropped.rows * EImageCropped.cols;
    string movmentName;
    
    
    //run on all options
    for(i =0; i < dataOptions; i++) {
        printf("comparing to : %s \n",+names[i].c_str());
        
        //run on all MEI of option
        for (j=0; j < numOfMEIPerDataOption; j++) {
            string currentAdd = "data/MEI/movments/" +names[i] + "" + to_string((j+1)) + ".png";
            
            
            //compare to ETimage
            Mat compareImage = imread( currentAdd, CV_LOAD_IMAGE_GRAYSCALE );
            Rect compareImagePointsOfIntrest= findBiggestBlob(compareImage);
            Mat compareImageCropped = compareImage(compareImagePointsOfIntrest);
            
            
            //resize to EImageCropped size
            Size size(EImageCropped.cols,EImageCropped.rows);
            resize(compareImageCropped,compareImageCropped,size);
            
            
            //test
            fileName ="data/MEI/" +names[i] + "" + to_string((j+1)) + ".png";
            imwrite( fileName,compareImageCropped);
            //end test
            
            //compare images
            int distanceBetweenImages =norm(EImageCropped,compareImageCropped); //eucliden distance = l2 distnace
            
            printf("-%i \n",distanceBetweenImages);
            
            if(distanceBetweenImages < minResult) {
                minResult =distanceBetweenImages;
                movmentName =names[i];
            }
            
        }
        
        printf("\n");
        
        
    }
    
    
    printf("detecting that current move is %s \n", movmentName.c_str());
    
    
    waitKey(0); // key press to close window
}













