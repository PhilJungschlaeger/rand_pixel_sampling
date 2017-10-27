#ifndef RSF_WRITER_HPP
#define RSF_WRITER_HPP

#include <string>
#include "pixel.hpp"

class RsfWriter
{

public:
  RsfWriter(std::string const& file);

public:
  void add(Pixel const& p);
  void save();
  void clean();
  bool exists(Pixel const& p);

private:
  std::string file_;
  int samples_;
  std::vector<Pixel> data_;
};
#endif
