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

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    int numOfMEIPerDataOption = 3;
    int dataOptions = 3;
    string names[3] = {"crunch","hands","sit"};
    
    //run on all options
    for(int x =0; x < dataOptions; x++) {
        //run on all MEI of option
        for (int z=0; z < numOfMEIPerDataOption; z++) {
            string fileName = "data/original_video/" +names[x] + "" + to_string((z+1)) + ".MOV";
            
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
            
            
            
            
            
            //save ET - last one only
            fileName ="data/MEI/" +names[x] + "" + to_string((z+1)) + ".png";
            imwrite( fileName,EImage);
            
            printf("finished \n");

            
        }
        
    }
    
    
    
    
    
    waitKey(0); // key press to close window
}




