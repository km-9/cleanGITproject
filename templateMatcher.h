#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

namespace templateMatch
{
  class templateMatcher
  {
    public:
      //if the matching method uses a mask or not
      bool use_mask;

      //img: the frame captured from the camera
      Mat img;
      //templ: current template being used by matching method
      Mat templ;
      //mask: mask for template (if any)
      Mat mask;
      //result: the resulting image after processing
      Mat result;

      //template of left side of crib
      Mat leftTempl;

      //right side of crib
      Mat rightTempl;

      //face/front of crib
      Mat frontTempl;

      //scaling multiplyer for template
      double scale = 0.7;

      //determines which side of the crib is being seen (according to which template is being used)
      int side;

      //always set to 4.
      int match_method;

      //calls getFrame(), checks if it was successful and then calls MatchingMethod;
      void lookAround();

    private:
      //runs the matching method and processes the image
      void MatchingMethod( int, void* );

      //returns a frame from a video stream
      Mat getFrame();
  };
}
