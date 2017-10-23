#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
using namespace cv;

class Pixel
{
public :

  Pixel(unsigned int = 0, unsigned int = 0);
  void print(std::ostream&) const;

  unsigned int x;
  unsigned int y;
  Vec3b color;
};

std::ostream& operator<<(std::ostream&, const Pixel&);

#endif
