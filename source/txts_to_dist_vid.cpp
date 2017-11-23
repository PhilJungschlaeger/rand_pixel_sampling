//simply reads txt-like pixeldata and draws a picture without interpretation

#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <time.h>             //time measuring & seed
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>   //image operations
#include <time.h>               //time measuring & seed
#include <string.h>
#include "pixel.hpp"
#include <math.h>
#include <vector>

int X=20; //von 540
int Y=10; //von 360

int SIZEX;
int SIZEY;

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
calcs a pixel(x,y)'s bucket N
*/
int xy_to_N(int x, int y)
{
  int N;
  N=std::floor((double)x*((double)X/((double)SIZEX)))+X*std::floor(((double)y*((double)Y/((double)SIZEY))));
  return N;
}

/*
distance, between two pixels
*/
double dist(int x1, int x2, int y1, int y2)
{
  double distance;
  distance=sqrt(std::pow((double)x1-x2,2)+std::pow((double)y1-y2,2)); //pow: nur pythagoras!!!
  return distance;
}

/*
calculates the closest distance of a pixel, from a box n, to a new box n_new
*/
double closest_dist(int x, int y, int n, int n_new)
{
  double n_d;
  if(n%X==n_new%X)  //gleiche spalte
  {
    if(n>n_new) //drüber
    {
      n_d=std::abs((((n_new+X)/X)*((double)SIZEY/((double)Y)))-y);
    }else{      //drunter
      n_d=std::abs(((n_new/X)*((double)SIZEY/((double)Y)))-y);
    }
  }else if(n/X==n_new/X) //gleiche reihe
  {
    if(n<n_new) //rechts
    {
      n_d=std::abs((n_new%X)*((double)SIZEX/((double)X))-x);
    }else{      //links
      n_d=std::abs(x-(((n_new+1)%X)*((double)SIZEX/((double)X))));
    }
  }else if(n%X<n_new%X)//rechts liegend
  {
    if(n/X>n_new/X) //oberhlab
    {
      //closest-point: linksunten
      int x_n=(n_new%X)*(SIZEX/X);
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //closest-point: links oben
      int x_n=(n_new%X)*(SIZEX/X);
      int y_n=(n_new/X)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }
  }else{  //links liegend
    if(n/X>n_new/X) //oberhlab
    {
      //closest-point: rechtsunten:
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //closest-point: rechts oben
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X))*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }
  }

  if(n_d>1){//for safety reasons...lieber nen bucket zu viel besuchen.
    n_d-=1; //nötig?
  }
  return n_d;
}

/*
check_bucket() sorts all count-closest points of v_in towards Pixel(x,y)
in v_out!
*/

void check_bucket(int x, int y, std::vector<Pixel> & v_in, std::list<std::pair<double,Vec3b> > & v_out, int& count)
{
  for(int p = 0; p < v_in.size(); ++p) {
    double distance=dist(v_in[p].x,x,v_in[p].y,y);  //distance of new pixel
    std::pair<double,Vec3b> to_store(distance,v_in[p].color);
    if(v_out.size())  //if v_out has already samples:
    {
      bool inserted=0;  //for later check
      for(std::list<std::pair<double,Vec3b> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {
        //achtung mehrdeutigkeit: wenn zwei pixel gleichweit entfernt sind.. welcher wird als n
        //nächster in betracht gezogen?!?
        bool doit=false;
        if(distance<=o->first)
        {
          if(distance==o->first)  //achtun mehrdeutigkeit
          {
            if(o->second[0]!=to_store.second[0]){
              if(o->second[0]>to_store.second[0])
              {
                doit=true;
              }
            }else if(o->second[1]!=to_store.second[1]){
              if(o->second[1]>to_store.second[1])
              {
                doit=true;
              }
            }else{  //same color is okay!
              if(o->second[2]>to_store.second[2])
              {
                doit=true;
              }
            }


          }else{  //echt kleiner
            doit=true;
          }
          if(doit)
          {
            inserted=1;
            v_out.insert(o,to_store); //problem?  //////////////////////////77
            break;
          }
        }
      }

      if((inserted==0)&&(v_out.size()<count)) //not inserted, but still not enough smaples.
      {
        v_out.push_back(to_store);
      }
    }else{  //always store first sample!:
      v_out.push_back(to_store);
    }

    if(v_out.size()>count) //liste zu groß?
    {
      v_out.pop_back();
    }
  }
}

//////////////////////////////////////
/*
ermittelt für einen pixel x,y, die count-viele nächste sample-punkte(aus v_in)
und übergibt einen vektor per referenz mit deren entfernungen und farben.
(dank bucket-system ist das ganze etwas unübersichtlich, aber effizient.)
*/
void closest_points(std::vector<std::vector<Pixel> > & v_in, int x, int y, int n, int& count, std::list<std::pair<double,Vec3b> > & v_out)
{
  int first_box=n;                                  //to save the origin-box
  std::list<std::pair<double,int> > sorted_buckets; //to stores buckets sorted, that might to be noted_boxes
  std::vector<int> noted_boxes;                     //stores already noted_boxes

  //origin-box:
  sorted_buckets.push_back(std::pair<double,int>(0.0,n));
  noted_boxes.push_back(n);
  double farest=99999999;                           //should be changed to infinity?

  while((sorted_buckets.size()))  //-as long, as there are buckets, to be visited left
  {                               //-farest-sample is farrer than closest-bucket
                                  //-or if we haven't found enough samples in the previous
                                  //  buckets?
    if(((farest>=sorted_buckets.begin()->first)|(v_out.size()<count)))
    {
      n=sorted_buckets.begin()->second; //take closest bucket
      check_bucket(x, y, v_in[n], v_out, count);//only vec, no: n, v_in

      std::list<std::pair<double,Vec3b> >::iterator la=v_out.end();
      --la; //bessere lösung?
      farest=la->first;
      sorted_buckets.pop_front(); //kick just visited bucket fronm list

      //find new buckets(4 are maximum possible):
      for(int i=0; i<4;i++) //
      {
        std::pair<double,int> box;//new bucket
        int new_box;              //new n
        double n_d;               //distance to n

        bool skip=false;          //check if box is necessary

        if(i==0){//bucket over last bucket
          new_box=n-X;
          if(new_box<0) //out of image
          {
            skip=true;
          }
        }else if(i==1)//bucket right-next to last bucket
        {
          new_box=n+1;
          if((new_box>=X*Y)|(((int) (n/X))!=((int) (new_box/X))))
          {
            skip=true;
          }
        }else if(i==2)//bucket under last bucket
        {
          new_box=n+X;
          if(new_box>=X*Y)
          {
            skip=true;
          }
        }else if(i==3)//bucket left-next to last bucket
        {
          new_box=n-1;
          if((new_box<0)|(((int) (n/X))!=((int) (new_box/X))))
          {
            skip=true;
          }
        }

        //is new bucket already noted?:
        for(std::vector<int>::iterator p = noted_boxes.begin(); p != noted_boxes.end(); ++p) {
          if(new_box==*p)
          {
            skip=true;
            break;
          }
        }

        if(!skip){//take new bucket:

          noted_boxes.push_back(new_box);           //note bucket
          n_d=closest_dist(x,y,first_box,new_box);  //calc closest distance from pixel(x,y) to bucket
          std::pair<double,int> box=std::pair<double,int>(n_d,new_box); //the new bucket

          bool inserted=0;
          if(sorted_buckets.size()) //sort bucket into list of to be visited buckets:..
          {
            for(std::list<std::pair<double,int> >::iterator o = sorted_buckets.begin(); o != sorted_buckets.end(); ++o)
            {
              if(o->first>box.first)
               {
                 inserted=1;
                 sorted_buckets.insert(o,box);
                 break;
               }
            }
            if(inserted==0)
            {
              sorted_buckets.push_back(box);
            }
          }else
          {
            sorted_buckets.push_back(box);
          }
        }
      }
    }else //we want to leave while-loop:
    {
      sorted_buckets.clear();
    }
  }
}

Mat read_image(std::string file, int count, int power)
{
  std::vector<std::vector<Pixel> > samples; //buckets
  for(int x=0; x<X; x++)
  {
    for(int y=0; y<Y; y++)
    {
      std::vector<Pixel> vec;
      samples.push_back(vec);
    }
  }

  //READ INPUT
  std::string line;
  std::ifstream myfile(file.c_str());
  if (myfile.is_open())
  {
      std::cout <<"open file" <<"\n";
      getline(myfile,line);
      std::stringstream meta;
      meta<<line;
      //RESOLUTION:
      meta>>SIZEX;
      meta>>SIZEY;

      Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));   //output-image
      Mat check_pic(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));//easy-trick, to remember, which pixel is sampled
                                                          //might be changed to a 2D array of booleans?

      while (getline(myfile,line))  //read whole input data:
      {
        std::stringstream ss;
        ss<<line;                   //First Line in
        std::string firstWord;

        int N;
        //Coordinat/Pixel:
        int x;
        int y;
        ss>>x;
        ss>>y;

        //Color:
        int r;
        int g;
        int b;
        ss>>r;
        ss>>g;
        ss>>b;

        Pixel pix;
        pix.x=x;
        pix.y=y;
        pix.color[0]=r;
        pix.color[1]=g;
        pix.color[2]=b;

        N=xy_to_N(x, y);  //get bucket
        samples[N].push_back(pix);            //store in bucket!

        //draw color to ouput:
        output.at<Vec3b>(Point(x,y))[0]=r;
        output.at<Vec3b>(Point(x,y))[1]=g;
        output.at<Vec3b>(Point(x,y))[2]=b;

        //for later check:
        check_pic.at<Vec3b>(Point(x,y))[0]=100; //already sampled
      }
      myfile.close();
      std::cout<<"samples are ready!\n";
  //-...........................................

  //PROCESS DATA:
      for(int x=0; x<SIZEX; x++)
      {
        for(int y=0; y<SIZEY; y++)
        {
          if(!check_pic.at<Vec3b>(Point(x,y))[0]) // if not allready sampled:
          {
            //std::cout<<"x "<<x<<" y "<<y<<"\n";
            int n=xy_to_N(x, y);

            std::list<std::pair<double,Vec3b> >  nec_points;    //neccesseray points
            closest_points(samples, x, y,n, count, nec_points); //get closest_points
            if(count==1)  //specialcas: we dont want to divide by zero!
            {
              //only closest:
              output.at<Vec3b>(Point(x,y))=nec_points.begin()->second;//floor(r+0.5);
            }else{

              //sum of distances:
              int sum=0;
              for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                sum+=p->first;
              }

              //calculate faktor: //to ensure, that each pixel has 100% intensity
              double faktor=0.0;
              for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                double dist= p->first;
                faktor+=std::pow((sum-dist),power);
              }

              //calc color:
              float r=0;
              float g=0;
              float b=0;

              for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                r+=p->second[0]*std::pow((sum-p->first),power)/(faktor);
                g+=p->second[1]*std::pow((sum-p->first),power)/(faktor);
                b+=p->second[2]*std::pow((sum-p->first),power)/(faktor);
              }

              output.at<Vec3b>(Point(x,y))[0]=r;
              output.at<Vec3b>(Point(x,y))[1]=g;
              output.at<Vec3b>(Point(x,y))[2]=b;
            }
          }
        }
      }
      return(output);
    }

}

int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH

    //const clock_t begin_time = clock(); //for time measurement
    if ( argc != 4 )//|| argc != 4)
    {
      std::cout<<"fnumber of param wrong\n";
      //USAGE?
      return -1;
    }

    std::string input= argv[1]; //folderinput
    char *endptr1;
    int count=strtol(argv[2], &endptr1, 10);//per parameter oben definieren und experimentieren!
    if (*endptr1 != '\0') {
      std::cout<<"second parameter was not an integer\n";
      return -1;
    }
    char *endptr2;
    int power=strtol(argv[3], &endptr2, 10);//per parameter oben definieren und experimentieren!
    if (*endptr2 != '\0') {
      std::cout<<"third parameter was not an integer\n";
      return -1;
    }


    std::vector<std::string> files = std::vector<std::string>();
    //std::vector<cv::Mat> pictures = std::vector<cv::Mat>();
    getdir(argv[1],files);
    //clock_t begin_time = clock();

    Size frameSize(static_cast<int>(640), static_cast<int>(360));
    VideoWriter oVideoWriter ("./output.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object

    int kra=0;
    for ( std::vector<std::string>::iterator p = files.begin(); p != files.end(); p++ ) {
      std::cout<<"frame: "<<kra<<"\n";
      oVideoWriter.write(read_image(*p, count, power));
      kra++;
    }

//MAIN:

//writer the frame into the file

    std::cout <<"done\n";
    return 0;
}
