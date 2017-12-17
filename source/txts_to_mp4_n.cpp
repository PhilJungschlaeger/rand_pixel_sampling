//simply reads txt-like pixeldata and draws a picture without interpretation

#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <time.h>             //time measuring & seed
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>

using namespace cv;
bool wayToSort(std::string i, std::string j)
{

  int i_number = 0;
  int j_number = 0;
  std::string firstString= "";
  std::string secondString="";

  std::stringstream ss1(i);
  ss1 >> firstString >> i_number;
  std::stringstream ss2(j);
  ss2 >> secondString >> j_number;
  //std::cout << "firstInteger " << firstIntege
  return i_number < j_number;
 }


 void getdir (std::string dir, std::vector<std::string> &files)
 {
     DIR *dp;
     struct dirent *dirp;
     if((dp  = opendir(dir.c_str())) == NULL) {
         std::cout << "Error(" << errno << ") opening " << dir << std::endl;
         //return errno;
     }

     while ((dirp = readdir(dp)) != NULL) {

       std::stringstream ss;
       if((std::string(".")).compare(dirp->d_name)&&(std::string("..").compare(dirp->d_name)))
       {
         std::string ending= ".txt";
         //.avi prohib
         if (ending.size() > std::string(dirp->d_name).size()){
         }else if(std::equal(ending.rbegin(),ending.rend(), std::string(dirp->d_name).rbegin()))
         {
           ss << dir;
           ss << dirp->d_name;
           std::string name;
           name=ss.str();
           files.push_back(name);
         }else{
           std::cout<<std::string(dirp->d_name)<<"\n";
           std::cout<<"proh\n";
         }

       }else{
         std::cout<<"proh\n";
       }
     }
     closedir(dp);

     std::sort( files.begin(), files.end(), wayToSort );

     //files.erase(files.begin());
     //files.erase(files.begin());
 }
/*
void getdir (const char* dir, std::vector<const char*> &files)
{
    DIR *dp;
    struct dirent *dirp;
    std::string direc=dir;

    if((dp  = opendir(direc.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        //return errno;
    }


    while ((dirp = readdir(dp)) != NULL) {
      std::string nam=dir+std::string(dirp->d_name);
      const char* file= new char[nam.size()];
      file=nam.c_str();

      //file=nam.c_str();
      files.push_back(file);
      std::cout<<files[0]<<"\n";

    }
    closedir(dp);
}
*/


void read_image(std::string file, Mat& output){
  std::string line;
  std::ifstream myfile(file.c_str());
  int SIZEX;
  int SIZEY;
  //std::cout<<"here""\n";
  std::cout<<file<<"\n";
  if (myfile.is_open())
  {
    //std::cout<<"here\n";
      //std::cout <<"open file" <<"\n";
      getline(myfile,line);
      std::stringstream meta;
      meta<<line;
      meta>>SIZEX;
      meta>>SIZEY;

    //  Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
      int x;
      int y;
      int r;
      int g;
      int b;
      Mat check_pic(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));//easy-trick, to remember, which pixel is sampled

      while (getline(myfile,line))
      {
          std::stringstream ss;
          ss<<line;                   //First Line in
          std::string firstWord;
          ss>>x;              //First Word in
          ss>>y;
          ss>>r;
          ss>>g;
          ss>>b;
          output.at<Vec3b>(Point(x,y))[0]=r;
          output.at<Vec3b>(Point(x,y))[1]=g;
          output.at<Vec3b>(Point(x,y))[2]=b;
      }


      myfile.close();

  }
}


int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH
    if ( argc != 2 )//|| argc != 4)
    {
        return -1;
    }


    std::vector<std::string> files = std::vector<std::string>();
    //std::vector<cv::Mat> pictures = std::vector<cv::Mat>();
    getdir(argv[1],files);
    clock_t begin_time = clock();

    Size frameSize(static_cast<int>(1280), static_cast<int>(720));
    VideoWriter oVideoWriter ("./output.avi", CV_FOURCC('P','I','M','1'), 50, frameSize, true); //initialize the VideoWriter object

    Mat last(720, 1280, CV_8UC3, Scalar(0,0,0));
    for ( std::vector<std::string>::iterator p = files.begin(); p != files.end(); p++ ) {
      //std::cout<<*p<<"\n";
      read_image(*p,last);
      oVideoWriter.write(last);
    }

//MAIN:

//writer the frame into the file

    std::cout <<"done\n";
    return 0;
}
