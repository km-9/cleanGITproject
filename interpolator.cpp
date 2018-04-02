#include <cstdlib>
#include <iostream>
#include <cassert>
#include "interpolator.hpp"
using namespace std;

double privateInterp(const point& left, const point& right, double angle) {
  if (left.r == 0 or right.r == 0) return 0;
  return (angle - left.theta)/(right.theta - left.theta) *
    (right.r - left.r) + left.r;
}

void interpolate(const vector<point>& inputPoints, int output[],
		 size_t n, double delta) {

  // special case: 0 degrees uses first and last data point
  point left = inputPoints.back();
  point right = inputPoints.front();
  left.theta -= 360;  // produces a negative angle
  right.theta += 360;

  vector<point> points;
  points.push_back(left);
  points.insert(points.end(), inputPoints.begin(), inputPoints.end());
  points.push_back(right);

  size_t currentLeftIndex = 0;

  double angle = delta;
  for(size_t i = 0; i < n; i++) {
    // search for two points that bracket angle
    while(currentLeftIndex < points.size() - 1 &&
	  !(
	    points[currentLeftIndex].theta < angle
	    && points[currentLeftIndex + 1].theta >= angle))
      currentLeftIndex++;
    left = points[currentLeftIndex];
    right = currentLeftIndex == points.size() - 1 ? farRight : points[currentLeftIndex + 1];

    // debugging output...make sure angle is correct
    //cout << left.theta << " < " << angle << " <= " << right.theta << endl;
    assert(left.theta < angle and angle <= right.theta);
    output[i] = privateInterp(left, right, angle);
    angle += delta;
  }
}
