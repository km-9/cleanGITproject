#include "templateMatcher.h"
namespace templateMatch
using namespace std;
using namespace cv;
/*
All usable variables are set to public for easy access, this can be switched to
proper getters/setters later, but for now KISS is all 10 commandments
*******************************************************************************
This program takes a sample image and a set of template images and attempts to
find the best match between the different templates are various sizes. If a
certainty value is too low, the program will ignore any potential crib objects.
If multiple templates or sizes return high certainty values the program will
pick the one with the highest value. This program differentiates between the
left, right, and front. of the crib object.
*******************************************************************************
End game of this should be a series of values returned which provide the robot
with a location for the crib, a distance to the crib, and a certainty that the
crib is, in fact, there. Currently it simply finds the crib and returns nothing.
*******************************************************************************
This program is currently being tested for a series of values. Currently, it
uses small template files and further reduces their size with resizing methods.
(from .3 to .7 in .05 intervals)
Eventually the "box" bounding the crib should be used as a bounding box for
a camShift method. This should allow the robot to align itself with the crib
with minmal difficulty and realative accuracy. (an offset of a few degrees/cm is
highly likely)
*/

templateMatch::templateMatcher()
{
  //TODO these filepaths are VERY wrong
  leftTempl = imread( "/home/kmnine/Desktop/Capstone/TemplateMatching/smallSize/Left.jpg", IMREAD_COLOR );
  rightTempl = imread( "/home/kmnine/Desktop/Capstone/TemplateMatching/smallSize/Right.jpg", IMREAD_COLOR );
  frontTempl = imread( "/home/kmnine/Desktop/Capstone/TemplateMatching/smallSize/Center.jpg", IMREAD_COLOR );
  scale = 0.7;
  match_method = 4;
}

//This is the only thing that should be called from outside (hence everything else being private)
//runs the template matching program, can be modified to return any relevant type as needed
void lookAround()
{
        img = getFrame();
        templ = frontTempl;

        if(img.empty() || templ.empty() || (use_mask && mask.empty()))
        {
                cout << "Can't read one of the images" << endl;
                return;
        }
        MatchingMethod( 0, 0 );
        return;
}

//creates a video capture and returns a single frame from it as a Mat
//can be modified to only construct a single videoCapture (look into better constructor for it)
Mat getFrame()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        cout << "Capture cannot be opened" << endl;

    Mat edges;
    Mat frame;
    namedWindow("edges",1);
    for(;;)
    {
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return frame;
}

//The actually matching method, this processes the image and generates/set relevant information
void MatchingMethod( int, void* )
{
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        double currMinVal; double currMaxVal; Point currMinLoc; Point currMaxLoc;
        Point matchLoc;
        double j = 0.3;
        ostringstream ss;

        if(img.empty() || templ.empty() || (use_mask && mask.empty()))
        {
                cout << "Can't read one of the images" << endl;
                return;
        }

        for(int i = 0; i <= 2; i++) {
                while (j <= 2) {
                        if (i == 0)
                                templ = frontTempl;
                        else if (i == 1)
                                templ = leftTempl;
                        else if (i == 2)
                                templ = rightTempl;
                        resize(templ, templ, Size(), j, j);
                        int result_cols =  img.cols - templ.cols + 1;
                        int result_rows = img.rows - templ.rows + 1;
                        result.create( result_rows, result_cols, CV_32FC1 );
                        bool method_accepts_mask = (CV_TM_SQDIFF == match_method || match_method == CV_TM_CCORR_NORMED);
                        if (use_mask && method_accepts_mask)
                        { matchTemplate( img, templ, result, match_method, mask); }
                        else
                        { matchTemplate( img, templ, result, match_method); }
                        minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                        if (currMaxVal <= maxVal ) {
                                side = i;
                                currMinVal = minVal;
                                currMaxVal = maxVal;
                                currMinLoc = minLoc;
                                currMaxLoc = maxLoc;
                        }
                        cout<<"i " << i << "| j "<< j << "| minVal "<< minVal<<"| maxVal "<< maxVal<<"| minLoc "<<minLoc<<"| maxLoc "<<maxLoc<<"| currMax "<<currMaxVal << "| side "<<side<<endl;
                        j += 0.05;
                }
                j = 0.3;
        }
//cout << endl;
//cout<<"Capture Number " << x << endl;
//}
        if (currMaxVal/100000000 < 1.5) {
                if (side == 0){
                        cout<<"front match";
                        cout<<matchLoc<<endl;
                      }
                else if (side == 1){
                        cout<<"left match";
                        cout<<matchLoc<<endl;
                      }
                else if (side == 2){
                        cout<<"right match";
                        cout<<matchLoc<<endl;
                      }
                else
                        cout << "Match too low" << endl;
                        return;
        }
        if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
        { matchLoc = currMinLoc; }
        else
        { matchLoc = currMaxLoc; }
        cout<<"side "<< side << "| minVal "<< currMinVal<<"| maxVal "<< currMaxVal<<"| minLoc "<<currMinLoc<<"| maxLoc "<<currMaxLoc<<"| currMax "<<currMaxVal << endl;
        if (side == 0)
                cout<<"front match";
        else if (side == 1)
                cout<<"left match";
        else if (side == 2)
                cout<<"right match";
        return;
}
