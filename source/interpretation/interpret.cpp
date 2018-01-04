#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <time.h>             //time measuring & seed


using namespace cv;

class Interpreter{
public:
  Interpreter(int X, int Y):
      _X(X),
      _Y(Y),
      _X_Buckets(160), //maybe clever function?
      _Y_Buckets(90)  //clever function?
      {}

      //no INTERPRETATION
  Mat no_interpretation(){
    std::cout<<"interpretation: no\n";
    Mat output(_Y, _X, CV_64FC3, Scalar(0,0,0));
    for(std::vector<Pixel>::iterator i = _Pattern.begin(); i != _Pattern.end(); ++i) {
      output.at<Vec3d>(Point((*i).x,(*i).y))=(*i).color;
    }
    return output;
  }
      //voronoi
      Mat voronoi(){
        return naive_proximity(1,1);
      }

      //splatting

      //splatting+voronoi

      //simple proximity !new radius!
      Mat naive_proximity(int count, int power){
        std::cout<<"interpretation: naive"<<count<<" "<<power<<"\n";
        Mat output=no_interpretation();

        for(int x=0; x<_X; x++)
        {
          for(int y=0; y<_Y; y++)
          {
            if(!_Check_pic.at<Vec3b>(Point(x,y))[0]) // if not allready sampled:
            {
              std::cout<<"x "<<x<<" y "<<y<<"\n";
            std::list<std::vector<double> > nec_points;    //neccesseray points
              closest_points(x, y,count, nec_points); //get closest_points


              /* shadow:::.
              std::vector<std::vector<int> > shadows;
              std::vector<int> indi;
              int ika=0;
              for(std::list<std::vector<double> >::iterator o = nec_points.begin(); o != nec_points.end(); ++o)
              {
                bool in_shadow=false;
                for(int p=0; p<shadows.size();p++ )
                {

                  //std::cout<<(shadows[p][0])<<"\n";
                  //*((*o)[0])+(shadows[p][1])*((*o)[1])<<">?"<<(shadows[p][2])<<"\n";
                  if((shadows[p][0])*((*o)[0])+(shadows[p][1])*((*o)[1])>(shadows[p][2]))
                  {

                    in_shadow=true;//

                    //std::cout<<"shadow!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    break;
                  }
                }

                if(in_shadow)
                {
                    indi.push_back(ika); //delete later!
                  //delete sample
                }else{
                  //add new shadow_space
                  //ortho:

                  double n_x=(*o)[0]-x;
                //  std::cout<<(*o)[0]<<"as\n";
                //  std::cout<<x<<"ass\n";
                  double n_y=(*o)[1]-y;
                  std::vector<int> new_shad;
                  new_shad.push_back(n_x);//x_fac;
                  new_shad.push_back(n_y);//y_fac;
                  new_shad.push_back((*o)[0]*n_x+(*o)[1]*n_y);//bordert_val;
                  shadows.push_back(new_shad);
                }
                ika++;
              }

              //delete shadow samples:

              for(int i=indi.size()-1; i>=0; i--)
              {
                std::list<std::vector<double> >::iterator la=nec_points.begin();
                for(int kr=0; kr<indi[i];kr++)
                {
                  la++;
                }
                nec_points.erase(la);
              }
              */

              if(count==1)  //special case: we dont want to divide by zero!
              {
                //only closest:
                output.at<Vec3d>(Point(x,y))[0]=(*nec_points.begin())[3];
                output.at<Vec3d>(Point(x,y))[1]=(*nec_points.begin())[4];
                output.at<Vec3d>(Point(x,y))[2]=(*nec_points.begin())[5];
                //floor(r+0.5);
              }else{

                /*
                //distance:
                double dist_fac=1;
                double angl_fac=1-dist_fac;
                int power_b=1;
                //sum of distances:
                int sum=0;  //now use max+min...
                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  sum+=(*p)[2];
                }
                */

                //sum of distances:
                //int sum=(*nec_points.begin())[2]+nec_points.back()[2]; new!!!
                //sum of distances:
                int sum=0;
                /*old:
                for(std::list<std::pair<double,Vec3d> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  sum+=p->first;
                }
                */
                sum=(*nec_points.begin())[2]+nec_points.back()[2];

                  /*angle.....*/

                //calculate faktor: //to ensure, that each pixel has 100% intensity
                double faktor=0.0;
                for(  std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  double dist= (*p)[2];
                  faktor+=std::pow((sum-dist),power);
                }

                //calc color:


                float r=0;
                float g=0;
                float b=0;

                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {

                  double influence=(std::pow(sum-(*p)[2],power))/faktor;//sum)*(
                  r+=(*p)[3]*influence;
                  g+=(*p)[4]*influence;
                  b+=(*p)[5]*influence;

                }

                //std::cout<<"fak"<<r<<"\n";
                output.at<Vec3d>(Point(x,y))[0]=r;
                output.at<Vec3d>(Point(x,y))[1]=g;
                output.at<Vec3d>(Point(x,y))[2]=b;
              }
            }
          }
        }
        return output;
      }

      //////////////////////////////////////
      /*
      ermittelt für einen pixel x,y, die count-viele nächste sample-punkte(aus _Buckets)
      und übergibt einen vektor per referenz mit deren entfernungen und farben.
      (dank bucket-system ist das ganze etwas unübersichtlich, aber effizient.)
      */
    //::: std::list<std::vector<double> > & v_out)

      void closest_points( int x, int y, int& count, std::list<std::vector<double> > & v_out)
      {
        int n=xy_to_N(x, y);  //momentary box
        int first_box=n;                                  //to save the origin-box
        std::list<std::pair<double,int> > sorted_buckets; //to stores buckets sorted, that might to be noted_boxes
        std::vector<int> noted_boxes;                     //stores already noted_boxes

        //origin-box:
        sorted_buckets.push_back(std::pair<double,int>(0.0,first_box));
        noted_boxes.push_back(first_box);
        double farest=99999999;                           //should be changed to infinity?

        while((sorted_buckets.size()))  //-as long, as there are buckets, to be visited left
        {                               //-farest-sample is farrer than closest-bucket
                                        //-or if we haven't found enough samples in the previous
                                        //  buckets?
          if(((farest>=sorted_buckets.begin()->first)|(v_out.size()<count)))
          {
            n=sorted_buckets.begin()->second; //take closest bucket
            check_bucket(x, y, _Buckets[n], v_out, count);//only vec, no: n, _Buckets

            if(v_out.size())    //schon einen sample?
            {
              std::list<std::vector<double> >::iterator la=v_out.end();
              --la; //bessere lösung?
              farest=((*la)[2]);
            }


            sorted_buckets.pop_front(); //kick just visited bucket fronm list

            //find new buckets(4 are maximum possible):
            for(int i=0; i<4;i++) //
            {
              std::pair<double,int> box;//new bucket
              int new_box;              //new n
              double n_d;               //distance to n

              bool skip=false;          //check if box is necessary

              if(i==0){//bucket over last bucket
                new_box=n-_X_Buckets;
                if(new_box<0) //out of image
                {
                  skip=true;
                }
              }else if(i==1)//bucket right-next to last bucket
              {
                new_box=n+1;
                if((new_box>=_X_Buckets*_Y_Buckets)|(((int) (n/_X_Buckets))!=((int) (new_box/_X_Buckets))))
                {
                  skip=true;
                }
              }else if(i==2)//bucket under last bucket
              {
                new_box=n+_X_Buckets;
                if(new_box>=_X_Buckets*_Y_Buckets)
                {
                  skip=true;
                }
              }else if(i==3)//bucket left-next to last bucket
              {
                new_box=n-1;
                if((new_box<0)|(((int) (n/_X_Buckets))!=((int) (new_box/_X_Buckets))))
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



      /*
      check_bucket() sorts all count-closest points of v_in towards Pixel(x,y)
      in v_out!
      */

      void check_bucket(int x, int y, std::vector<Pixel> & v_in, std::list<std::vector<double> > & v_out, int& count)
      {
        for(int p = 0; p < v_in.size(); ++p) {
          double distance=dist(v_in[p].x,x,v_in[p].y,y);  //distance of new pixel
          // x, y, dist, r, g, b
          std::list<std::vector<double> > nec_points;
          std::vector<double> to_store;
          to_store.push_back(v_in[p].x);
          to_store.push_back(v_in[p].y);
          to_store.push_back(distance);
          to_store.push_back(v_in[p].color[0]);
          to_store.push_back(v_in[p].color[1]);
          to_store.push_back(v_in[p].color[2]);

          if(v_out.size())  //if v_out has already samples:
          {
            bool inserted=0;  //for later check

            for(std::list<std::vector<double> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {
              //achtung mehrdeutigkeit: wenn zwei pixel gleichweit entfernt sind.. welcher wird als n
              //nächster in betracht gezogen?!?
              bool doit=false;
              if(distance<=((*o)[2]))
              {
                if(distance==((*o)[2])) //achtun mehrdeutigkeit
                {
                  if(((*o)[3])!=to_store[3]){  //r
                    if(((*o)[3])>to_store[3])
                    {
                      doit=true;
                    }
                  }else if(((*o)[4])!=to_store[4]){
                    if(((*o)[4])>to_store[4])
                    {
                      doit=true;
                    }
                  }else{  //same color is okay!
                    if(((*o)[5])>to_store[5])
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

/*
      void check_bucket(int x, int y, std::vector<Pixel> & v_in, std::list<std::pair<double,Vec3d> > & v_out, int& count)
      {
        for(int p = 0; p < v_in.size(); ++p) {
          double distance=dist(v_in[p].x,x,v_in[p].y,y);  //distance of new pixel
          std::pair<double,Vec3d> to_store(distance,v_in[p].color);
          if(v_out.size())  //if v_out has already samples:
          {
            bool inserted=0;  //for later check
            for(std::list<std::pair<double,Vec3d> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {
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
      */

      /*
      calculates the closest distance of a pixel, from a box n, to a new box n_new
      */
      double closest_dist(int x, int y, int n, int n_new)
      {
        double n_d;
        if(n%_X_Buckets==n_new%_X_Buckets)  //gleiche spalte
        {
          if(n>n_new) //drüber
          {
            n_d=std::abs((((n_new+_X_Buckets)/_X_Buckets)*((double)_Y/((double)_Y_Buckets)))-y);
          }else{      //drunter
            n_d=std::abs(((n_new/_X_Buckets)*((double)_Y/((double)_Y_Buckets)))-y);
          }
        }else if(n/_X_Buckets==n_new/_X_Buckets) //gleiche reihe
        {
          if(n<n_new) //rechts
          {
            n_d=std::abs((n_new%_X_Buckets)*((double)_X/((double)_X_Buckets))-x);
          }else{      //links
            n_d=std::abs(x-(((n_new+1)%_X_Buckets)*((double)_X/((double)_X_Buckets))));
          }
        }else if(n%_X_Buckets<n_new%_X_Buckets)//rechts liegend
        {
          if(n/_X_Buckets>n_new/_X_Buckets) //oberhlab
          {
            //closest-point: linksunten
            int x_n=(n_new%_X_Buckets)*(_X/_X_Buckets);
            int y_n=((n_new/_X_Buckets)+1)*(_Y/_Y_Buckets);
            n_d=dist(x,x_n,y, y_n);
          }else{          //unterhalb
            //closest-point: links oben
            int x_n=(n_new%_X_Buckets)*(_X/_X_Buckets);
            int y_n=(n_new/_X_Buckets)*(_Y/_Y_Buckets);
            n_d=dist(x,x_n,y, y_n);
          }
        }else{  //links liegend
          if(n/_X_Buckets>n_new/_X_Buckets) //oberhlab
          {
            //closest-point: rechtsunten:
            int x_n=((n_new%_X_Buckets)+1)*(_X/_X_Buckets);
            int y_n=((n_new/_X_Buckets)+1)*(_Y/_Y_Buckets);
            n_d=dist(x,x_n,y, y_n);
          }else{          //unterhalb
            //closest-point: rechts oben
            int x_n=((n_new%_X_Buckets)+1)*(_X/_X_Buckets);
            int y_n=((n_new/_X_Buckets))*(_Y/_Y_Buckets);
            n_d=dist(x,x_n,y, y_n);
          }
        }

        if(n_d>1){//for safety reasons...lieber nen bucket zu viel besuchen.
          n_d-=1; //nötig?
        }
        return n_d;
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
      Mat output(_X, _Y, CV_64FC3, Scalar(0,0,0));
      for(std::vector<Pixel>::iterator i = _Pattern.begin(); i != _Pattern.end(); ++i) {
        output.at<Vec3d>(Point((*i).x,(*i).y))[0]=(*i).color;
      }
      */
      //((Mat output(X, Y, CV_8UC3, Scalar(0,0,0)); //FLOAT IMAGE!
      //shadow proximity

      //area

      //area and shadow proximity

      //area and shadow proximity + splatting

      /*
      calcs a pixel(x,y)'s bucket N
      */
      int xy_to_N(int x, int y)
      {
        int N;
        N=std::floor((double)x*((double)_X_Buckets/((double)_X)))+_X_Buckets*std::floor(((double)y*((double)_Y_Buckets/((double)_Y))));
        return N;
      }

      void set_pattern(std::vector<Pixel>& pattern){
        _Pattern=pattern;

         _Check_pic=Mat(_Y, _X, CV_8UC3, Scalar(0,0,0));//might be changed to a 2D array of booleans?
        //Prepare Buckets:
        /*correct image scale? */
        std::cout<<"!WARNING! input image has to fit bucket-sys!\n";
        //best Bucket amount?
        //int samples_amount=_Pattern.size();
        for(int x=0; x<_X_Buckets; x++)
        {
          for(int y=0; y<_Y_Buckets; y++)
          {
            std::vector<Pixel> vec;
            _Buckets.push_back(vec);
          }
        }


        int N;
        for(std::vector<Pixel>::iterator i = _Pattern.begin(); i != _Pattern.end(); ++i) {
          N=xy_to_N((*i).x, (*i).y);  //get bucket
          _Buckets[N].push_back(*i);
          _Check_pic.at<Vec3b>(Point((*i).x, (*i).y))[0]=100; //already sampled
        }
      }

private:
  int _X;
  int _Y;
  int _X_Buckets;
  int _Y_Buckets;
  std::vector<Pixel> _Pattern;
  std::vector<std::vector<Pixel> > _Buckets;
  Mat _Check_pic;
};

//using namespace cv;
//sample_video
//sample_picture
//file:
