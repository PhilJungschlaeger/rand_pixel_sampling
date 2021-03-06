#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <time.h>             //time measuring & seed
#include <math.h>             //ceil, sqrt etc


using namespace cv;

class Sampler{
public:
  //expects CV_8UC3 image!
  Sampler(int amount, Mat const& image):
      _Amount(amount),
      _Image(image)
      {
        srand(time(NULL));  //SEED
        _X=_Image.cols;
        _Y=_Image.rows;
      }

  std::vector<Pixel_d>  calc_grid(){
    std::cout<<"sampling grid\n";
    std::vector<Pixel_d> output_pattern;

    //SAMPLING
    cv::Size size = _Image.size();
    Pixel_d pix;
    double count = std::ceil(std::sqrt(_Amount));
    int y_spacing = (int)std::max(std::ceil(size.height/count),1.0);
    int x_spacing = (int)std::max(std::ceil(size.width/count),1.0);
    std::cout<<size.height<<std::endl;
    for(int x=0; x<size.width; x+=y_spacing)
    {
      for(int y=0; y<size.height; y+=y_spacing)
      {
        pix.x = x;
        pix.y = y;
        pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));
        output_pattern.push_back(pix);
      }
    }
    return output_pattern;
  }

  std::vector<Pixel_d>  calc_halton(){
    std::cout<<"sampling halton\n";
    std::vector<Pixel_d> output_pattern;

    //SAMPLING
    cv::Size size = _Image.size();
    int p = 2; int q = 3;
    Pixel_d pix;

    for (int i=1; i<_Amount+1; i++)
    {
      double fx = 1; double fy = 1; 
      int ix = i; int iy = i; 
      double rx = 0; double ry = 0;

      while (ix > 0) {
      	fx /= p;
      	rx += fx * (ix % p);
      	ix = ix / p;
      }

      while (iy > 0) {
      	fy /= q;
      	ry += fy * (iy % q);
      	iy = iy / q;
      }
      double x = rx * size.width;
      double y = ry * size.height;
      pix.x= x;
      pix.y= y;
      pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));

      output_pattern.push_back(pix);
    }
    return output_pattern;
  }

  //expects CV_8UC3 image!
  void set_image(Mat const& image){
    _Image=image;
  }



  std::vector<Pixel>  calc_rand(){
    std::cout<<"sampling random\n";
    std::vector<Pixel> output_pattern;

    //PREPARE:
    std::vector<std::pair<int, int> > not_sampled_yet;
    for(int x=0; x<_X; x++)
    {
      for(int y=0; y<_Y; y++)
      {
        not_sampled_yet.push_back(std::pair<int,int>(x,y));
      }
    }

    //SAMPLING
    Pixel pix;
    //srand(time(NULL));  //SEED
    for (int i=0; i<_Amount; i++)
    {

      int n= rand()% not_sampled_yet.size();
      pix.x= not_sampled_yet[n].first;
      pix.y= not_sampled_yet[n].second;
      pix.color = _Image.at<Vec3b>(Point(pix.x,pix.y));
      not_sampled_yet[n]=not_sampled_yet.back();
      not_sampled_yet.pop_back();
      output_pattern.push_back(pix);
    }
    return output_pattern;
  }

  std::vector<Pixel_d>  calc_rand_d(){
    std::cout<<"sampling random\n";
    std::vector<Pixel_d> output_pattern;

    //PREPARE:
    std::vector<std::pair<int, int> > not_sampled_yet;
    for(int x=0; x<_X; x++)
    {
      for(int y=0; y<_Y; y++)
      {
        not_sampled_yet.push_back(std::pair<int,int>(x,y));
      }
    }

    //SAMPLING
    Pixel_d pix;

    for (int i=0; i<_Amount; i++)
    {
      //std::cout<<i<<"\n";
      int n= rand()% not_sampled_yet.size();
      pix.x= not_sampled_yet[n].first;
      pix.y= not_sampled_yet[n].second;
      pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));
      not_sampled_yet[n]=not_sampled_yet.back();
      not_sampled_yet.pop_back();
      output_pattern.push_back(pix);
    }
    return output_pattern;
  }

private:
  int _Amount;
  Mat _Image;
  int _X;
  int _Y;
};

//using namespace cv;
//sample_video
//sample_picture
//file:
