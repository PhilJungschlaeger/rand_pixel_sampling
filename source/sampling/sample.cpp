#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <time.h>             //time measuring & seed


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

  /*
  std::vector<Pixel>  calc_grid(){
    std::cout<<"sampling grid\n";
    NOT YET IMPLEMENTED
  }
  */

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
