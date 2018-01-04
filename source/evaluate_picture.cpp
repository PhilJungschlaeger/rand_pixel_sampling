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
  int samples_amount=10000;

  Mat image;

  std::string input= argv[1];
  Mat input_image;
  input_image=imread( input,1);
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
  std::cout<<"01. SAMPLING\n";
  Sampler sampler(samples_amount, input_image);
  std::vector<std::vector<Pixel> > patterns;
  //patterns.push_back(sampler.calc_grid()); //0:GRID
  patterns.push_back(sampler.calc_rand()); //1:RAND
  //...

//02. INTERPRETATION
std::cout<<"02. INTERPRETATION\n";
std::vector<Mat> output_images;
Interpreter interpreter(input_image.cols,input_image.rows);
std::vector<std::string> methods;
for(std::vector<std::vector<Pixel> >::iterator pattern = patterns.begin(); pattern != patterns.end(); ++pattern) {

  interpreter.set_pattern(*pattern);
  methods.push_back("no_interp");
  output_images.push_back(interpreter.no_interpretation());
  methods.push_back("voronoi");
  output_images.push_back(interpreter.voronoi());
  methods.push_back("prox2_2");
  output_images.push_back(interpreter.naive_proximity(2,2));
}

//03. EVALUATION
std::cout<<"03. EVALUATION\n";
Evaluator evaluator(input_image);
for(int i=0; i<output_images.size(); i++) {
  std::cout<<i<<" "<<methods[i]<<"\n";
  imwrite("a"+std::string(methods[i])+std::string("eval_abs_")+".jpg",evaluator.evaluate_abs(output_images[i]));
  //imwrite("a"+std::string(methods[i])+std::string("eval_3d_")+".jpg",evaluator.evaluate_abs(output_images[i]));
  imwrite("a"+std::string(methods[i])+std::string("output")+".jpg", output_images[i] );
}


}
