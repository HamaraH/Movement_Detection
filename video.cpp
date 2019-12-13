#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "buffer.hpp"
#include "video.cpp"




TraitementVideo();
TraitementVideo(String ip);
TraitementVideo(String ip, String name);
~TraitementVideo();

bool presenceMouvement(){
  Mat image_diff, image_diff_gris, image_binaire;

  //différence absolue des 2 images en input
  absdiff(this->oldframe , this->newframe  , image_diff);

  //met l'image de différence en noir et blanc+ nuances de gris
  cvtColor( image_diff, image_diff_gris, COLOR_BGR2GRAY );

  //met l'image en noir et banc en format binaire noir/blanc ajuster au seuil de différence
  threshold(image_diff_gris,image_binaire,this->seuil,255,0);

  //donne le pourcentage de pixels blanc
  double pourcentage = (double)countNonZero(image_binaire)/(image_binaire.rows*image_binaire.cols);

  return pourcentage>=this->sensibility;
}
void flushBuffer(){
  if (! buffer->isEmpty()) {
      //obtient le buffer, il se peut que certaine cases du tab ne sois pas remplient
      Mat* tab = buffer->getBuffer();
      for (int i=0;i<buffer->getSize();i++) {
          if (tab[i].empty()) {
              break;
          }
          writer.write(tab[i]);
      }
      delete[] tab;
  }
}

VideoCapture getCapture(){
  return this->capture;
}
void setCapture(Videocapture capture){
  this->capture = capture
}


VideoWriter getWriter(){
  return this->writer;
}
void setWriter(VideoWriter writer){
  this->writer = writer;
}


Buffer getBuffer(){
  return this->buffer;
}
void setBuffer(Buffer buffer){
  this->buffer = buffer;
}


Mat getOldframe(){
  return this->oldframe;
}
void setOldframe(Mat oldframe){
  this->oldframe = oldframe;
}
void setOldframeCopy(Mat oldframe){
  this->oldframe = oldframe.clone();
}


Mat getNewframe(){
  return this->newframe;
}
void setNewframe(Mat newframe){
  this->newframe=newframe;
}
void setNewframeCopy(Mat newframe){
  this->newframe = newframe.clone();
}


int getSeuil(){
  return this->seuil;
}
void setSeuil(int seuil){
  this->seuil = seuil;
}


double getSensibility(){
  return this->sensibility;
}
void setSensibility(double sensibility){
  this->sensibility= sensibility;
}


String getIp(){
  return this->ip;
}
void setIp(String ip){
  this->ip = ip;
}


String getCameraName(){
  return this->cameraname;
}
String setCameraName(String cameraname){
    this->cemeraname = cameraname;
}


Size getSize(){
  return this->size;
}
void setSize(Size size){
  this->size = size;
}
void setSize(int length, int height){
  this->size =size(length, height);
}


double getFps(){
  return this->fps;
}
void setFps(double fps){
  this->fps=fps;
}


int getCodec(){
  return this->codex;
}
//le codec voulu est récupéré grace à la fonction static opencv VideoWriter::fourcc('X', 'X', 'X', 'X')
void setCodec(int codec){
  this->codec = codec;
}


void stop(){
  this.stop = 0;
}
