/*
info:
  the rsf writer writes given pixel, x-,y-coordinat and color(r,g,b) to a .txt
  like file.

todo:
  |necessary:
  -...?
  |optional:
  -further header information?
  -better data reservation/calculation for vector..
  -later we could code the information cheaper
*/
#include "rsf_writer.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>


RsfWriter::RsfWriter(std::string const& file):
  file_(file),
  samples_(0),
  data_()
  {clean();}

void RsfWriter::add(Pixel const& p)
{
  samples_++;
  data_.push_back(p);
}

void RsfWriter::save()
{
  std::fstream file(file_.c_str(), std::ios::out);
  //header:?
  file<<"samples: "<< samples_<<"\n";

  //data:
  for ( std::vector<Pixel>::iterator p = data_.begin(); p != data_.end(); p++ ) {
    file << p->x << ",";
    file << p->y << ";";
    file << (int)p->color[0] << ",";
    file << (int)p->color[1] << ",";
    file << (int)p->color[2] << ";\n";
  }
  file.close();
}

void RsfWriter::clean()
{
  std::fstream file(file_.c_str(), std::ios::out);
  file.clear();
  file.close();
}
