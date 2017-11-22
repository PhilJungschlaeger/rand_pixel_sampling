#include <stdio.h>
#include <opencv2/opencv.hpp>   //image operations
#include <time.h>               //time measuring & seed
#include <string.h>
#include "pixel.hpp"
#include <math.h>
#include <vector>
//#include <pair>
//choose wise: 674x674calc

//bucketing:
//chose X and Y as natural divider of the resolution!
int X=1;
int Y=1;//vs 7
//28 27 28.5
int SIZEX;
int SIZEY;

bool info=false;

//allow any function
using namespace cv;
int xy_to_N(int x, int y, int width, int height, int X, int Y)
{//grob getestet!
  int N;
  N=std::floor((double)x*((double)X/((double)SIZEX)))+X*std::floor(((double)y*((double)Y/((double)SIZEY))));
  //N=std::floor((x*X/(SIZEX))+std::floor(X*(y*(Y/(SIZEY))))); falsch

  if(info)
  {
      std::cout<<N<<" "<<X/SIZEX<<" \n";
  }

  return N;
}

double dist(int x1, int x2, int y1, int y2)
{
  double distance=sqrt(std::pow((double)x1-x2,2)+std::pow((double)y1-y2,2)); //pow: nur pythagoras!!!
  if(info)
  {
    std::cout<<x1-x2<<" "<<y1-y2<<" "<<distance<<" asifdsa\n";
  }

  return distance;
}

//double!=!?!?!?
double closest_dist(int x, int y, int n, int n_new)
{
  double n_d;
  if(n%X==n_new%X)  //gleiche spalte
  {
    //std::cout<<"gl. spalte!\n";
    if(n>n_new) //drüber
    {
      n_d=std::abs((((n_new+X)/X)*((double)SIZEY/((double)Y)))-y);      //check

      //std::cout<<"dr: "<<(n_new+X)/X<<" "<<(SIZEY/Y)<<" \n";
    }else{      //drunter

      //2/2 * (20/2) -0
      n_d=std::abs(((n_new/X)*((double)SIZEY/((double)Y)))-y);     //check
    }
  }else if(n/X==n_new/X) //gleiche reihe
  {
    //std::cout<<"gl. reihe!\n";
    if(n<n_new) //rechts
    {
      n_d=std::abs((n_new%X)*((double)SIZEX/((double)X))-x);    //check

    }else{      //links
      n_d=std::abs(x-(((n_new+1)%X)*((double)SIZEX/((double)X)))); //check
    }
  }else if(n%X<n_new%X)//rechts liegend
  {
    if(n/X>n_new/X) //oberhlab
    {
      //punkt: linksunten
      //std::cout<<"br: linksunt!\n";
      int x_n=(n_new%X)*(SIZEX/X);      //wasist mit krummen pixeln!? -> nur passend, erstmal!!!!!!!!!!!!!!
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //punkt: links oben

      int x_n=(n_new%X)*(SIZEX/X);
      int y_n=(n_new/X)*(SIZEY/Y);
      //std::cout<<"br: linksoben!:"<<x_n<<" "<<y_n<<"\n";
      n_d=dist(x,x_n,y, y_n);
    }

  }else{  //links liegend
    if(n/X>n_new/X) //oberhlab
    {
      //punkt: rechtsunten:
    //  std::cout<<"br: rechtsunt!\n";
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //punkt: rechts oben
      //std::cout<<"br: rechtsoben\n";
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X))*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }
  }
  if(n_d>1){//for safety reasons
    n_d-=1; //nötig?
  }
  return n_d;
}

/////////////////////////////////////////////////////////////!!
//: pair benutzen und nur zwischenspeichern, was wirkich wichtig ist!
// in der höheren ebene drüberusmieren!
void closest_points(std::vector<std::vector<Pixel> > & v_in, int x, int y, int n, int& count, std::list<std::pair<double,Vec3b> > & v_out)
{
  int first_box=n;
  std::list<std::pair<double,int> > bboxes;
  bboxes.push_back(std::pair<double,int>(0.0,n));
  std::vector<int> visited; //maybe change name.. stores buckets, that are/wer to be visited
  double farest=99999999;//infinity?
  //es gibt noch boxen?, die nähste box ist näher als der entfernteste, wir haben noch nicht genug
  int cnt=0;
  visited.push_back(n);

  while((bboxes.size()))
  {
    //std::cout<<bboxes.size()<<" size\n";
    //std::cout<<farest<<" far\n";
    if(((farest>=bboxes.begin()->first)|(v_out.size()<count)))
    {



    n=bboxes.begin()->second;
    if(info)
    {
      std::cout<<cnt<<" cnt: "<<n<<".....................\n";
    }





    //erstmal box checken:
    //kann mit einer liste vielleicht schöner/effizienter sein? oder einem cleveren vector sort..?

    for(int p = 0; p < v_in[n].size(); ++p) {    //  N-steps
    //  std::cout<<"here"<<n<<"d"<<p<<"\n";
    //  std::cout<<v_in[n][p].x<<"\n";
      double distance=dist(v_in[n][p].x,x,v_in[n][p].y,y);  //distance of new pixel

      std::pair<double,Vec3b> to_store(distance,v_in[n][p].color);

      if(v_out.size())
      {//der vektor hat schon inhalt:
        bool inserted=0;

        //lieber normale for-schleife? und dann checken, ob kleiner als count ist..
        for(std::list<std::pair<double,Vec3b> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {

          //achtung mehrdeutigkeit: wenn zwei pixel gleichweit entfernt sind.. welcher wird als n
          //nöchster in betracht gezogen?!?
          if(distance<=o->first) //sobald kleiner, inserten! noch verbessrern?
          {
            if(distance==o->first)
            {
              bool doit=false;
              //was ist, wenn gleich!?:

              if(o->second[0]!=to_store.second[0]){ //good to compare
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

              if(doit)
              {
                inserted=1;
                v_out.insert(o,to_store); //problem?  //////////////////////////77
                break;
              }
            }else{
              inserted=1;
              v_out.insert(o,to_store); //problem?  //////////////////////////77
              break;
            }

          }
        }

        if(inserted==0&&v_out.size()<count)
        {
          v_out.push_back(to_store);
        }

      }else{
        //der vektor ist noch leer:
        v_out.push_back(to_store);
      }


      if(v_out.size()>count) //das kürzen könnte man durhc vorherige clevernheit weglassen
      {
        v_out.pop_back();
      }

    }
  //  std::cout<<"here\n";
    std::list<std::pair<double,Vec3b> >::iterator la=v_out.end();
    --la;
    farest=la->first;
    //find sort in the 4 boxes
    //std::cout<<"pop "<<bboxes.begin()->second<<" \n";
    bboxes.pop_front();
    for(int i=0; i<4;i++)
    {
      std::pair<double,int> box;
      int new_box;

      double n_d;

      bool skip=false;
      //check if i is in range
      if(i==0){//drüber
        new_box=n-X;

        if(new_box<0)
        {
          skip=true;
        }
        /*
        else{
          //N=x*X/width+X*(y*Y/height);
          n_d=std::abs(y-(n/X)*(SIZEY/Y));  //opt: mid
          //opt: right
          //opt: left
          //std::cout<<"oben: "<<n_d<<"\n";
        }
        */
      }else if(i==1)//rechts
      {
        new_box=n+1;

        //std::cout<<((int) (n/X))<<" "<<((int) (new_box/X))<<"\n";
        if((new_box>=X*Y)|(((int) (n/X))!=((int) (new_box/X))))
        {
          skip=true;
        }
        /*else{

          //std::cout<<"rechts: "<<n_d<<"\n";
        }
      */
      }else if(i==2)//unten
      {
        new_box=n+X;
        if(new_box>=X*Y)
        {
          skip=true;
        }
        /*else{
          //N=x*X/width+X*(y*Y/height);
          n_d=std::abs(y-(n/X)*(SIZEY/Y));  //opt: mid drüber
          n_d=std::abs((new_box/X)*(SIZEY/Y)-y); //drunter
        //  std::cout<<"untenn: "<<n_d<<"\n";
        }
        */
      }else if(i==3)//links
      {
        new_box=n-1;

        if((new_box<0)|(((int) (n/X))!=((int) (new_box/X))))
        {
          skip=true;
        }
      }
        /*
      }else{
      n_d=std::abs((new_box%X)*(SIZEX/X)-x); rehcts
      n_d=std::abs(x-(n%X)*(SIZEX/X)); links
       //std::cout<<"links: "<<n_d<<"\n";
      }
      */



      for(std::vector<int>::iterator p = visited.begin(); p != visited.end(); ++p) {
        if(new_box==*p)
        {
        //  std::cout<<"visited\n";
          skip=true;
          break;
        }
      }


      if(!skip){
        visited.push_back(new_box);

        n_d=closest_dist(x,y,first_box,new_box);
        if(info)
        {
          std::cout<<"from"<<n<<"to "<<new_box<<"und "<<skip<<"\n";
          std::cout<<n_d<<" disttopix\n";
        }

        std::pair<double,int> box=std::pair<double,int>(n_d,new_box);
        bool inserted=0;

        if(bboxes.size())
        {
          for(std::list<std::pair<double,int> >::iterator o = bboxes.begin(); o != bboxes.end(); ++o)
          {
            if(o->first>box.first)
             {
               inserted=1;
               bboxes.insert(o,box); //problem?  //////////////////////////77
               break;
             }
          }
          if(inserted==0)
          {
            bboxes.push_back(box);
          }
        }else
        {
          bboxes.push_back(box);
        }
      }
    }
//  std::cout<<"bbsize"<<bboxes.size()<<"far: "<<farest<<"<?"<<bboxes.begin()->first<<"voutsiz: "<<v_out.size()<<"\n";

    cnt++;
  }else{
  //  std::cout<<"clear:\n";
    bboxes.clear();
  //  std::cout<<"sizllllllllllllllllllllllllllllllllllllllllllll:"<<bboxes.size()<<"\n";
  }
}
//std::cout<<cnt<<"cnt\n";

  if(v_out.size()!=count)
  {
    std::cout<<"size?: "<<v_out.size()<<"\n";
  }

}

int main(int argc, char** argv )
{

/* to check xy_to_N()
  X=50;
  Y=50;//vs 7
  //28 27 28.5
  SIZEX=500;
  SIZEY=500;

  for (int i=0; i<500; i++)
  {
    int N= xy_to_N(500-i,i,SIZEX,SIZEY,X,Y);
    std::cout<<i<<" N:"<<N<<"\n";
  }
  */

//CHECK_INPUT:
    //CHECK LENGTH

      const clock_t begin_time = clock();
    if ( argc != 4 )//|| argc != 4)
    {
      std::cout<<"fnumber of param wrongr\n";
        return -1;

    }

		std::string input= argv[1]; //file input
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

//MAIN:

    std::string line;
    std::ifstream myfile(argv[1]);


  //  std::vector<Pixel> samples; //vector with all samples
    std::vector<std::vector<Pixel> > samples; //buckets
    for(int x=0; x<X; x++)
    {
      for(int y=0; y<Y; y++)
      {
        std::vector<Pixel> vec;
      //  std::cout<<"here"<<vec.size()<<"\n";
        samples.push_back(vec);
      }
    }
    //std::vector<Pixel> not_sampled;
    //std::cout<<"here"<<samples[1].size()<<"\n";
    if (myfile.is_open())
    {
      //READ:
      //-...........................................
        std::cout <<"open file" <<"\n";
        getline(myfile,line);
        std::stringstream meta;
        meta<<line;
        //RESOLUTION:
        meta>>SIZEX;
        meta>>SIZEY;

        Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
        Mat check_pic(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
        while (getline(myfile,line))  //read whole input data:
        {
          std::stringstream ss;
          ss<<line;                   //First Line in
          std::string firstWord;

          int N;
          //Coordinat/Pixel:
          int x;
          int y;
          ss>>x;              //First Word in
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

          //store:  //for later use
          N=xy_to_N(x, y, SIZEX, SIZEY, X, Y);
          samples[N].push_back(pix);  //sort in bucket!
          //draw color to ouput:
          output.at<Vec3b>(Point(x,y))[0]=r;
          output.at<Vec3b>(Point(x,y))[1]=g;
          output.at<Vec3b>(Point(x,y))[2]=b;
          //for later check:
          check_pic.at<Vec3b>(Point(x,y))[0]=100; //already sampled
        }
        myfile.close();
        std::cout<<"samples are ready!\n";

        //std::cout<<"here"<<samples[0].size()<<"\n";
//-...........................................

//PROCESS DATA:
//-...........................................

//"
        //std::cout<<"here"<<samples[10][10].x<<"\n";
        //vlt. kann man folgende schleife irgendwie in die erste rienziehen um durchläufe zu sparen?

        for(int x=0; x<SIZEX; x++)
        {
          for(int y=0; y<SIZEY; y++)
          {

            //int x= 10;
            //int y= 10;

            if(!check_pic.at<Vec3b>(Point(x,y))[0]) // if value 100
            {//not sampled!
              std::cout<<"x "<<x<<" y "<<y<<"\n";

              int sum=0;
              double faktor=0.0;
              double check=0;
              std::list<std::pair<double,Vec3b> >  nec_points; //distance,color
              //calc sum and get <count> closest points:
              int n=xy_to_N(x, y, SIZEX, SIZEY, X, Y);
              //std::cout<<n<<"N\n";
              closest_points(samples, x, y,n, count, nec_points);
              if(count!=nec_points.size())
              {
                std::cout<<"heree\n";
              }
              if(count==1)  //we dont want to divide by zero!
              {
                //only closest:
                output.at<Vec3b>(Point(x,y))=nec_points.begin()->second;//floor(r+0.5);
              }else{//to check!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //afterwards, calc sum!
                for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  sum+=p->first;
                }

                //calculate faktor:
                for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  double dist= p->first;
                  faktor+=std::pow((sum-dist),power);
                }

                //calculate color using pixels, distances, and faktor
                float r=0;
                float g=0;
                float b=0;

                for(std::list<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  check+=std::pow((sum-p->first),power)/faktor;

                  r+=p->second[0]*std::pow((sum-p->first),power)/(faktor);
                  g+=p->second[1]*std::pow((sum-p->first),power)/(faktor);
                  b+=p->second[2]*std::pow((sum-p->first),power)/(faktor);
                }

                //std::cout<<"faktor: "<<faktor<<"\n";
                //std::cout<<"check:=1?=  "<<check<<"\n";
                if(info)
                {
                  std::cout<<r<<"r\n";
                  std::cout<<g<<"g\n";
                  std::cout<<b<<"b\n";
                }

                output.at<Vec3b>(Point(x,y))[0]=r;//floor(r+0.5);
                output.at<Vec3b>(Point(x,y))[1]=g;//floor(g+0.5);
                output.at<Vec3b>(Point(x,y))[2]=b;//floor(b+0.5);
              }
            }

          }
        }


        if(samples.size()<count)
        {
          std::cout<<"samples_count<influencin points\n";
        }
        imwrite( "output_dist_interpretation.jpg", output );
        //std::cout <<"done\n";
        std::cout <<"done\ntook:"<<(float( clock () - begin_time )/  CLOCKS_PER_SEC)<<"seconds in total\n";


        return 0;
  }

}
