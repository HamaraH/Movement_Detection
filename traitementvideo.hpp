#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <queue>

#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "buffer.hpp"


#ifndef BUFFER_H
#define BUFFER_H

using namespace std;
using namespace cv;

class TraitementVideo {

private:
  //variable pour la capture video
  VideoCapture capture;
  String url;

  //variable pour l'écriture de la video
  VideoWriter writer;
  String cameraname;
  Size size;
  double fps;
  int codec;

  //stock les images
  Buffer buffer;

  //variable de detection du mouvement
  Mat oldframe;
  Mat newframe;
  double sensibility;
  int seuil;

  ToWrite writeQueue;

  //permet de stoper la detection
  bool stop;


public:

TraitementVideo();
TraitementVideo(String url);
TraitementVideo(String url, String name);
TraitementVideo(String url, int seuil, double sensibility);
TraitementVideo(String url, String name, int seuil, double sensibility);
~TraitementVideo();

bool presenceMouvement();
void flushBuffer();
int traitement();
void toToWrite(queue<Mat> temp);
void * writeThread(void * arg);
void stop();
string getIp();
bool pingIp(string ipAdress);

VideoCapture getCapture();
void setCapture(Videocapture cap);

VideoWriter getWriter();
void setWriter(VideoWriter writer);

Buffer getBuffer();
void setBuffer(Buffer buffer);

Mat getOldframe();
void setOldframe(Mat oldframe);
void setOldframeCopy(Mat oldframe);

Mat getNewframe();
void setNewframe(Mat newframe);
void setNewframeCopy(Mat newframe);

int getSeuil();
void setSeuil(int seuil);

double getSensibility();
void setSensibility(double sensibility);

String getUrl();
int setUrl(String url);

String getCameraName();
String setCameraName(String cameraname);

Size getSize();
void setSize(Size size);
void setSize(int length, int height);

double getFps();
void setFps(double fps);

int getCodec();
void setCodec(int codec);






};

class ToWrite{

private:

  queue<Mat> queue;
  bool continueWrite;
  pthread_mutex mutex;
public:
  toWrite();
  ~toWrite();

  void setContinueWrite(bool);
  bool getContinueWrite();

  void mutexInit();
  void mutexBlock();
  void mutexOpen();

  queue<Mat> getQueue();
};



#endif
