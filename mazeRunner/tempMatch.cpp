#include "tempMatch.h"
using namespace std;
using namespace cv;
{
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        double currMinVal; double currMaxVal; Point currMinLoc; Point currMaxLoc;
        Point matchLoc;
        Mat img_display;
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
                                img.copyTo( img_display );
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
                if (side == 0)
                        cout<<"front match";
                else if (side == 1)
                        cout<<"left match";
                else if (side == 2)
                        cout<<"right match";
                cout << "Match too low" << endl;
                return;
        }
        if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
        { matchLoc = currMinLoc; }
        else
        { matchLoc = currMaxLoc; }
        rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols, matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
        rectangle( result, matchLoc, Point( matchLoc.x + templ.cols, matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
        imshow( image_window, img_display );
        imshow( result_window, result );
        cout<<"side "<< side << "| minVal "<< currMinVal<<"| maxVal "<< currMaxVal<<"| minLoc "<<currMinLoc<<"| maxLoc "<<currMaxLoc<<"| currMax "<<currMaxVal << endl;
        if (side == 0)
                cout<<"front match";
        else if (side == 1)
                cout<<"left match";
        else if (side == 2)
                cout<<"right match";
        return;
}

        //VideoCapture cap(CV_CAP_ANY);
    raspicam::RaspiCam_Cv cap;

        if( !cap.isOpened() )
        {
            cout << "Could not initialize capturing...\n";
            return 0;
        }

        while(1)
        {
          cap.grab();
          cap.retrieve(output);

          imshow("webcam input", output);
          char c = (char)waitKey(10);
          if( c == 27 ) break;
         }
