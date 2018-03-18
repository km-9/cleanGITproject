#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

namespace TemplateMatching
{
  class tempMatch
  {
  public:
    //constructor
    tempMatch();
      //the current maxVal for match assurance
      double maxVal;
      //the current minVal for match assurance
      double minVal;
      //value for template scaling
      double scaler;
      //the left template
      Mat leftTempl;
      //the center template
      Mat frontTempl;
      //the right template
      Mat rightTempl;
      //returns a video capture caught with default camera
      VideoCapture getVideoCap(0);
      //returns a single frame from videoCapture
      Mat getFrame();
  }
}
