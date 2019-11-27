#include <iostream>
#include <stdio.h>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


int main(int argc, char* argv[]){

  Mat frame;//= une image
  VideoCapture cap; // la video
  cap.open("http://192.168.0.2:8080/cam");
  if(cap.isOpened()){
    cout<<"cam bien chopée\n";
  }
  else{
    cout<<"cam mal chopée\n";
  }
}
