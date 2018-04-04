#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#include <iostream>

using namespace cv;
using namespace std;

Mat frame, blurColor, postBlur, mask, output;

  int findFire(Mat frame){
    GaussianBlur(frame, blurFrame, (21, 21), 0, 0, BORDER_DEFAULT);

    cvtColor(blurFrame, postColor, COLOR_BGR2HSV, 0)

    int lower [3] = { 18, 50, 50};
    int upper [3] = { 35, 255, 255};
    inRange(postColor, lower, upper, mask);

    bitwise_and(frame, postColor, output, mask)

    thresh_callback(0, 0);

    return 0;
  }

  void drawRect(Mat src){
   Mat threshold_output;
   vector<vector<Point> > contours;
   vector<Vec4i> hierarchy;

   threshold( src, threshold_output, thresh, 255, THRESH_BINARY );
   findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

   vector<vector<Point> > contours_poly( contours.size() );
   vector<Rect> boundRect( contours.size() );
   vector<Point2f>center( contours.size() );

   vector<float>radius( contours.size() );
   for( size_t i = 0; i < contours.size(); i++ )
   {
     approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
     boundRect[i] = boundingRect( Mat(contours_poly[i]) );
     minEnclosingCircle( contours_poly[i], center[i], radius[i] );
   }

   Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );

   for( size_t i = 0; i< contours.size(); i++ )
   {
     Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
     drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
     rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
     circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
   }

   namedWindow( "Contours", WINDOW_AUTOSIZE );
   imshow( "Contours", drawing );
 }
