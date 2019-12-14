#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <list>
#include <queue>

#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "buffer.hpp"
#include "traitementvideo.cpp"

using namespace std;
using namespace cv;

TraitementVideo(){
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
}

TraitementVideo::TraitementVideo(String ip){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  //ping bon
  Mat data;
  this->capture.open(ip);
  this->capture.read(data);

  this->size= Size(data.cols,data.rows);
  double temp = this->capture.get(CAP_PROP_FPS);
  // on regarde si les fps sont valide car la fonction peut bug
  if(temps>120){
    temp =20;
  }
  this->setFps(temp);

  this->name = this.capture.getBackendName();
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();
  // ping pas bon -> prévenir l'utilisateur
}

TraitementVideo::TraitementVideo(String ip, String name){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  // ping
  Mat data;
  this->capture.open(ip);
  this->capture.read(data);

  this->setSize(data.cols,data.rows);
  tdouble temp = this->capture.get(CAP_PROP_FPS);
  // on regarde si les fps sont valide car la fonction peut bug
  if(temps>120){
    temp =20;
  }
  this->setFps(temp);
  this->name = name;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::TraitementVideo(String ip, int seuil, double sensibility){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  // ping
  Mat data;
  this->capture.open(ip);
  this->capture.read(data);

  this->setSize(data.cols,data.rows);
  double temp = this->capture.get(CAP_PROP_FPS);
  // on regarde si les fps sont valide car la fonction peut bug
  if(temps>120){
    temp =20;
  }
  this->setFps(temp);
  this->name = this.capture.getBackendName();
  this->seuil = seuil;
  this->sensibility = sensibility;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::TraitementVideo(String ip, String name, int seuil, double sensibility){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  // ping
  Mat data;
  this->capture.open(ip);
  this->capture.read(data);

  this->setSize(data.cols,data.rows);
  double temp = this->capture.get(CAP_PROP_FPS);
  // on regarde si les fps sont valide car la fonction peut bug
  if(temps>120){
    temp =20;
  }
  this->setFps(temp);

  this->name = name;
  this->seuil = seuil;
  this->sensibility = sensibility;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::~TraitementVideo();// destructeur à faire

bool TraitementVideo::presenceMouvement(){
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
void TraitementVideo::flushBuffer(){
  if (! this->buffer.isEmpty()) {
      //obtient le buffer, il se peut que certaine cases du tab ne sois pas remplient
      Mat* tab = this->buffer.getBuffer();//remplacer le tab par un vecteur
      for (int i=0;i<this->buffer.getSize();i++) {
          if (tab[i].empty()) {
              break;
          }
          this->writer.write(tab[i]);
      }
      delete[] tab;
  }
}

int TraitementVideo::traitement(){

  if(! this->capture.isOpened()){
    //ping -> si bon
    this->capture.open(this->ip);
    //si pas bon afficher erreur et return -1
  }

  if (! this->capture.isOpened()) {
    //regarde si le stream mal ouvert
    cout<<this->cameraname<<" cam mal chopée\n";
    return -1;
  }

  // créer le répertoire de stockage de video
  if (chdir(this->cameraname.c_str())==0){
    chdir("../");
  }
  else{
    mkdir(this->cameraname.c_str());// faire un truc qui regarde si il existe pas déjà, si il existe pas: le crée + mettre les permissions
  }

  //prend la première image -> pas de traitement nécéssaire
  this->capture.read(this->oldframe);

  // pour voir le temps d'exec
  //clock_t t1, t2;
  //float temps;

  while(this->stop) {

    //  t1 = clock();

    this->capture.read(this->newframe);

    if (this->newframe.empty()) {
      cout <<yhis->cameraname<< " frame mal chopée\n";
      return -1;
    }

    //partie traitement des images

    if (this->presenceMouvement()) {
      //créer et ouvrir le fichier video si il n'est pas ouvert
      if (! this->writer.isOpened()) {
        //je met la partie du titre (date+heure+.format)
        // le nom du fichier sera la date et l'heure
        time_t tmm = time(0);
        tm* now = localtime(&tmm);
        std::stringstream ss;
        ss <<this->cameraname<<"/"<< now->tm_mday << "-" << now->tm_mon+1 << "-" << now->tm_year+1900 << ":" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec << ".avi";
        string nomfichier = ss.str();

        this->writer.open(nomfichier,this->codec,this->fps,this->size,true);
      }

      //ajout du contenue du buffer et de l'image du mouvement
      // + vérification si le writer est ouvert pour plus de sécurité
      if(this->writer.isOpened()){
        this->flushBuffer();
        this->buffer.clearBuffer();
        //ajouter l'image
        this->writer.write(this->newframe);
      }
    }



    else {
      //non mouvement

      //ajout de newframe au buffer
      buffer->addMat(newframe.clone());

      //cas pour la fermeture du fichier video
      if (buffer->isFull() && writ.isOpened()) {
        flushBuffer(writ,buffer);
        writ.release();
      }
    }

    oldframe.release();
    oldframe=newframe.clone();

    /*  t2 = clock();
    temps = (float) (t2-t1)/ CLOCKS_PER_SEC;
    printf("temps d'exec = %f \n", temps);*/
  }

  return 0;

}


void TraitementVideo::toToWrite(queue<Mat> temp){
  while(! temp.empty()){
    this->toWrite.push(temp.front());
    temp.pop();
  }
}

void * TraitementVideo::writeThread(void * arg){
  TraitementVideo * data = (TraitementVideo *) arg;
  // check si le writer est ouvert : wait

  //ouvrir le Writer

  while(data->continueWrite){
    if(! data->toWrite.empty()){
      data->Writer
    }
  }
  //mutex la struc de la queue toWrite
  //copy le bousin
  //finir le write avec la copy
}



void TraitementVideo::stop(){
  this.stop = 0;
}







VideoCapture TraitementVideo::getCapture(){
  return this->capture;
}
void TraitementVideo::setCapture(Videocapture capture){
  this->capture.release();
  this->capture = capture
}


VideoWriter TraitementVideo::getWriter(){
  return this->writer;
}
void TraitementVideo::setWriter(VideoWriter writer){
  this->writer.release();
  this->writer = writer;
}


Buffer TraitementVideo::getBuffer(){
  return this->buffer;
}
void TraitementVideo::setBuffer(Buffer buffer){
  // detruire le buffer
  this->buffer = buffer;
}


Mat TraitementVideo::getOldframe(){
  return this->oldframe;
}
void TraitementVideo::setOldframe(Mat oldframe){
  this->oldframe.release();
  this->oldframe = oldframe;
}
void TraitementVideo::setOldframeCopy(Mat oldframe){
  this->oldframe.release();
  this->oldframe = oldframe.clone();
}


Mat TraitementVideo::getNewframe(){
  return this->newframe;
}
void TraitementVideo::setNewframe(Mat newframe){
  this->newframe.release();
  this->newframe=newframe;
}
void TraitementVideo::setNewframeCopy(Mat newframe){
   this->newframe.release();
  this->newframe = newframe.clone();
}


int TraitementVideo::getSeuil(){
  return this->seuil;
}
void TraitementVideo::setSeuil(int seuil){
  this->seuil = seuil;
}


double TraitementVideo::getSensibility(){
  return this->sensibility;
}
void TraitementVideo::setSensibility(double sensibility){
  this->sensibility= sensibility;
}


String TraitementVideo::getIp(){
  return this->ip;
}
//réinitialise tous les paramètres précédament assigné sauf le nom et le codec
int TraitementVideo::setIp(String ip){
  delete this->ip;
  this->ip = ip;
  //reconfig les params (comme dans le constructeur)
}


String TraitementVideo::getCameraName(){
  return this->cameraname;
}
String TraitementVideo::setCameraName(String cameraname){
  delete this->cameraname;
    this->cemeraname = cameraname;
}


Size TraitementVideo::getSize(){
  return this->size;
}
void TraitementVideo::setSize(Size size){
  this->size = size;
}
void TraitementVideo::setSize(int length, int height){
  this->size =size(length, height);
}


double TraitementVideo::getFps(){
  return this->fps;
}
void TraitementVideo::setFps(double fps){
  // génère un nouveau buffer
  this->fps=fps;
  this->buffer.setSize((int)fps*2);
}


int TraitementVideo::getCodec(){
  return this->codex;
}
//le codec voulu est récupéré grace à la fonction static opencv VideoWriter::fourcc('X', 'X', 'X', 'X')
void TraitementVideo::setCodec(int codec){
  this->codec = codec;
}






















// class toWrite


ToWrite::toWrite(){
  this->mutex = PTHREAD_MUTEX_INITIALIZER;
}
ToWrite::~toWrite(){
  this->queue.~queue();
}

void ToWrite::setContinueWrite(bool value){
  this->continueWrite=value;
}
bool ToWrite::getContinueWrite(){
  return this->continueWrite;
}

void ToWrite::mutexInit(){
  this->mutex = PTHREAD_MUTEX_INITIALIZER;
}

void ToWrite::mutexBlock(){
  pthread_mutex_lock(&this->mutex);
}
void ToWrite::mutexOpen(){
  pthread_mutex_unlock(&this->mutex);

}

queue<Mat> ToWrite::getQueue(){
  return this->queue;
}
