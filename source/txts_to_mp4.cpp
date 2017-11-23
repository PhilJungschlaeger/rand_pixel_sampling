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
      ss << dir;
      ss << dirp->d_name;

      std::string name;
      name=ss.str();
      files.push_back(name);
    }
    closedir(dp);
    std::sort( files.begin(), files.end() );
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

Mat read_image(std::string file)
{
  std::string line;
  std::ifstream myfile(file.c_str());
  int SIZEX;
  int SIZEY;
  std::cout<<"here""\n";
  std::cout<<file<<"\n";
  if (myfile.is_open())
  {
    std::cout<<"here\n";
      std::cout <<"open file" <<"\n";
      getline(myfile,line);
      std::stringstream meta;
      meta<<line;
      meta>>SIZEX;
      meta>>SIZEY;

      Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
      int x;
      int y;
      int r;
      int g;
      int b;

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

      return output;
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

    Size frameSize(static_cast<int>(640), static_cast<int>(360));
    VideoWriter oVideoWriter ("./output.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object


    for ( std::vector<std::string>::iterator p = files.begin(); p != files.end(); p++ ) {

      oVideoWriter.write(read_image(*p));
    }

//MAIN:

//writer the frame into the file

    std::cout <<"done\n";
    return 0;
}
