#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <time.h>             //time measuring & seed
#include <math.h>       /* sqrt */

using namespace cv;

/*
expects double images, not integers!*/
class Evaluator{
public:
 Evaluator(Mat const& ref_image):
      _Ref_image(ref_image)
      {}

  Mat evaluate_abs(Mat const& image)
  {
    //red is wrong
    double max=255*3;
    double min=0;
    //blue is correct
    double sum=0;
    Mat output(_Ref_image.rows, _Ref_image.cols, CV_64FC3, Scalar(0,0,0));
    for(int x=0; x<_Ref_image.cols; x++)
    {
      for(int y=0; y<_Ref_image.rows; y++)
      {
        double error=0;
        for (int c=0; c<3; c++)
        {
          error+=abs(((double)_Ref_image.at<Vec3b>(Point(x,y))[c])-image.at<Vec3d>(Point(x,y))[c]);
        }
        sum+=error;
        if(error<255)
        {
          output.at<Vec3d>(Point(x,y))[0]=error;
        }else if(error<510)
        {
          output.at<Vec3d>(Point(x,y))[0]=255;
          output.at<Vec3d>(Point(x,y))[1]=(error-255);
        }else
        {
          output.at<Vec3d>(Point(x,y))[0]=255;
          output.at<Vec3d>(Point(x,y))[1]=255;
          output.at<Vec3d>(Point(x,y))[2]=(error-2*255);
        }


        /*
        output.at<Vec3d>(Point(x,y))[2]=error/((double)3);
        //output.at<Vec3d>(Point(x,y))[1]=255.0f-error/((double)3);
        output.at<Vec3d>(Point(x,y))[0]=255.0f-error/((double)3);
        //output.at<Vec3d>(Point(x,y))[2]=255.0f-error/((double)3);
        //double r=
        */

      }
    }
    std::cout<<"error_abs:"<<sum<<"\n";
    return output;
  }

  Mat evaluate_3d(Mat const& image)
  {
    //red is wrong
    double max=255*sqrt(3);
    double min=0;
    //blue is correct
    double sum=0;
    Mat output(_Ref_image.rows, _Ref_image.cols, CV_64FC3, Scalar(0,0,0));
    for(int x=0; x<_Ref_image.cols; x++)
    {
      for(int y=0; y<_Ref_image.rows; y++)
      {
        double error=0;
        for (int c=0; c<3; c++)
        {
          error+=pow(_Ref_image.at<Vec3b>(Point(x,y))[c]-image.at<Vec3d>(Point(x,y))[c],2);
        }
        error=sqrt(error);
        sum+=error;
        output.at<Vec3d>(Point(x,y))[0]=error/((double)sqrt(3));
        output.at<Vec3d>(Point(x,y))[2]=255.0f-error/((double)sqrt(3));;
      }
    }
    std::cout<<"error_3D:"<<sum<<"\n";
    return output;
  }

private:
  Mat _Ref_image;
};

//using namespace cv;
//sample_video
//sample_picture
//file:
