#include "sampling/sample.cpp"
#include "interpretation/interpret.cpp"
#include "evaluation/evaluate.cpp"
#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <string>
#include "pixel.hpp"

using namespace cv;

int main(int argc, char** argv )
{
  /*
  Im folgenden wird verglichen welches Verfahren am meisten Informationen vom
  ursprungsbild interpoliert.
  einfache zufallsverteilung oder gridverteilung oder ...
  */
  //SAMPLING
  int samples_amount=102400;//1024000; //25%

  Mat image;

  std::string input= argv[1];
  Mat input_img=imread( input,1);
  Mat input_image;
  input_img.convertTo(input_image, CV_64FC3);
  std::size_t pos = input.find(".");      // position of "live" in str
  std::string name = std::string(input.substr (0,pos));
  std::cout<<"ref_image: "<<name<<"\n";
  std::cout<<"samples: "<<samples_amount<<"\n";
  name+=samples_amount;
  if ( !input_image.data )
  {
      std::cout<<"found no image data \n";
      return -1;
  }

//01. SAMPLING
  std::cout<<"\n01.°°°°°°°°°°°°°°°°°°°°°°°°SAMPLING\n";
  Sampler sampler(samples_amount, input_image);
  std::vector<std::vector<Pixel_d> > patterns;
  //patterns.push_back(sampler.calc_grid()); //0:GRID
  patterns.push_back(sampler.calc_rand_d()); //1:RAND
  //...

//02. INTERPRETATION
std::cout<<"\n02.°°°°°°°°°°°°°°°°°°°°°°°°INTERPRETATION\n";
std::vector<Mat> output_images;
Interpreter interpreter(input_image.cols,input_image.rows);
std::vector<std::string> methods;
for(std::vector<std::vector<Pixel_d> >::iterator pattern = patterns.begin(); pattern != patterns.end(); ++pattern) {

  interpreter.set_pattern(*pattern);
  /*methods.push_back("no_interp");
  output_images.push_back(interpreter.no_interpretation());
  */
  methods.push_back("voronoi");
  output_images.push_back(interpreter.voronoi());
  methods.push_back("prox2_2");
  /*
  output_images.push_back(interpreter.naive_proximity(2,2));
  methods.push_back("shadowa");
  output_images.push_back(interpreter.shadow_proximity(0));
  methods.push_back("shadowb");
  output_images.push_back(interpreter.shadow_proximity(1));
  methods.push_back("shadowc");
  output_images.push_back(interpreter.shadow_proximity(2));
  methods.push_back("areaonly0");
  output_images.push_back(interpreter.area_only_proximity(0));
  methods.push_back("areaandprox0");
  output_images.push_back(interpreter.area_and_proximity(0));
  methods.push_back("areaonly1");
  output_images.push_back(interpreter.area_only_proximity(1));
  methods.push_back("areaandprox1");
  output_images.push_back(interpreter.area_and_proximity(1));
  */

}

//03. EVALUATION
std::cout<<"\n03.°°°°°°°°°°°°°°°°°°°°°°°°EVALUATION\n";
Evaluator evaluator(input_image);
for(int i=0; i<output_images.size(); i++) {
  std::cout<<i<<" "<<methods[i]<<"\n";
  Mat out=evaluator.evaluate_abs(output_images[i]);
  imwrite("b"+std::string("eval_abs_")+std::string(methods[i])+".jpg",out);
  //imwrite("a"+std::string(methods[i])+std::string("eval_3d_")+".jpg",evaluator.evaluate_abs(output_images[i]));
  imwrite("b"+std::string("output")+std::string(methods[i])+".jpg", output_images[i] );
}


}
