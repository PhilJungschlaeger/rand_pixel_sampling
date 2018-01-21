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

  Mat evaluate_abs(Mat const& image, std::fstream & stream)
  {
    //red is wrong
    double max=255*3;
    double min=0;
    //blue is correct
    double sum=0;
    std::vector<std::pair<double,int> > borders;
    double val=0.0;
    double step=0.002;
    borders.push_back(std::pair<double,int>(0,0));    //correct Pixel
    /*
    borders.push_back(std::pair<double,int>(0.001,0));
    borders.push_back(std::pair<double,int>(0.002,0));
    borders.push_back(std::pair<double,int>(0.005,0));
    borders.push_back(std::pair<double,int>(0.01,0));
    borders.push_back(std::pair<double,int>(0.1,0));
    */
    borders.push_back(std::pair<double,int>(0.5,0));  //inbetween
    borders.push_back(std::pair<double,int>(1.0,0));  //bad Pixel
    /*
    while(val<1.0)
    {
      borders.push_back(std::pair<double,int>(val,0));
      val+=0.002;
    }
    */

    Mat output(_Ref_image.rows, _Ref_image.cols, CV_64FC3, Scalar(0,0,0));
    for(int x=0; x<_Ref_image.cols; x++)
    {
      for(int y=0; y<_Ref_image.rows; y++)
      {
        double error=0;
        for (int c=0; c<3; c++)
        {
          error+=abs(((double)_Ref_image.at<Vec3d>(Point(x,y))[c])-image.at<Vec3d>(Point(x,y))[c]);
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


        for(std::vector<std::pair<double,int> > ::iterator o = borders.begin(); o !=  borders.end(); ++o) {
          if(error<= max*(*o).first)
          {
            (*o).second++;
            break;
          }
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
    stream<<"\tsum:"<<sum;
    int total=0;
    //std::vector<Mat> eval(1280,border.size(), CV_64FC3, Scalar(0,0,0));

    //int coll=0;
    for(std::vector<std::pair<double,int> > ::iterator o = borders.begin(); o !=  borders.end(); ++o) {

      total+=(*o).second;
      /*
      for(int i=0; i<(*o).second; i++)
      {
        eval.at<Vec3d>(Point(coll,i))[0]=255;
      }
      coll++;*/
      std::cout<<"better than: "<<(*o).first<<": "<<total<<"got:"<<(*o).second<<"\n";
      stream<<"\t"<<(*o).first<<": "<<(*o).second;
    }
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
          error+=pow(_Ref_image.at<Vec3d>(Point(x,y))[c]-image.at<Vec3d>(Point(x,y))[c],2);
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
