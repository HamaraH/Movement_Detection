#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <list>
#include <queue>
#include <fstream>
#include <vector>
#include <cstdlib>

#include <pthread.h>
#include <unistd.h>


#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traitementvideo.hpp"

using namespace std;
using namespace cv;

TraitementVideo::TraitementVideo(){
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();
}

TraitementVideo::TraitementVideo(String url){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url=url;
  if(this->pingIp(this->getIp())){
    Mat imgex;
    this->capture.open(this->url);
    this->capture.read(imgex);

    this->size= Size(imgex.cols,imgex.rows);
    double temp = this->capture.get(CAP_PROP_FPS);
    // on regarde si les fps sont valide car la fonction peut bug
    if(temp>120){
      temp =20;
    }
    this->setFps(temp);

    this->cameraname = this->capture.getBackendName();
  }
  else{
    cout<<"ip non reconnue\n";
  }
  this->sensibility = 0.1;
  this->seuil= 25;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();
  // ping pas bon -> prévenir l'utilisateur
}

TraitementVideo::TraitementVideo(String url, String name){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url = url;
  if(this->pingIp(this->getIp())){
    Mat imgex;
    this->capture.open(url);
    this->capture.read(imgex);

    this->setSize(imgex.cols,imgex.rows);
    double temp = this->capture.get(CAP_PROP_FPS);
    // on regarde si les fps sont valide car la fonction peut bug
    if(temp>120){
      temp =20;
    }
    this->setFps(temp);
  }
  else{
    cout<<"ip non reconnue\n";
  }
  this->sensibility = 0.1;
  this->seuil= 25;
  this->cameraname = name;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::TraitementVideo(String url, int seuil, double sensibility){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url = url;
  if(this->pingIp(this->getIp())){
    Mat imgex;
    this->capture.open(url);
    this->capture.read(imgex);

    this->setSize(imgex.cols,imgex.rows);
    double temp = this->capture.get(CAP_PROP_FPS);
    // on regarde si les fps sont valide car la fonction peut bug
    if(temp>120){
      temp =20;
    }
    this->setFps(temp);
    this->cameraname = this->capture.getBackendName();
  }
  else{
    cout<<"ip non reconnue\n";
  }
  this->seuil = seuil;
  this->sensibility = sensibility;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::TraitementVideo(String ip, String name, int seuil, double sensibility){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url = url;
  if(this->pingIp(this->getIp())){
    Mat imgex;
    this->capture.open(url);
    this->capture.read(imgex);

    this->setSize(imgex.cols,imgex.rows);
    double temp = this->capture.get(CAP_PROP_FPS);
    // on regarde si les fps sont valide car la fonction peut bug
    if(temp>120){
      temp =20;
    }
    this->setFps(temp);
  }
  else{
    cout<<"ip non reconnue\n";
  }
  this->cameraname = name;
  this->seuil = seuil;
  this->sensibility = sensibility;
  this->codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  this->writeQueue.mutexInit();

}

TraitementVideo::~TraitementVideo(){
  this->capture.release();
  this->writer.release();
  this->oldframe.release();
  this->newframe.release();
}

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
      queue<Mat> tab = this->buffer.getBuffer();//remplacer le tab par un vecteur
      this->toToWrite(tab);
  }
}

void *TraitementVideo::traitement(void *arg){

  TraitementVideo * data = (TraitementVideo *) arg;

  if(data->getCameraName().empty()){
    data->setCameraName("defaultname");
  }

  if(! data->getCapture()->isOpened()){
    if(data->pingIp(data->getIp())){
      //initialise les les partie non initialisé : voir pour fair eune autre fonction ?
      data->setUrl(data->getUrl());
      data->getCapture()->open(data->getUrl());

      if (! data->getCapture()->isOpened()) {
        //regarde si le stream mal ouvert
        cout<<data->getCameraName()<<" cam mal chopée\n";
        pthread_exit(NULL);
      }
    }
    else{
      cout<<"url non reconnue\n";
      pthread_exit(NULL);
    }
    //si pas bon afficher erreur et return -1
  }

  // créer le répertoire de stockage de video
  if (chdir(data->getCameraName().c_str())==0){
    chdir("../");
  }
  else{
    mkdir(data->getCameraName().c_str(),S_IRWXU);// faire un truc qui regarde si il existe pas déjà, si il existe pas: le crée + mettre les permissions
  }

  //prend la première image -> pas de traitement nécéssaire
  data->readNextFrame();

  data->setCompteurThread(0);

  // pour voir le temps d'exec
  //clock_t t1, t2;
  //float temps;

  data->initstop();
  while(data->getContinueTraitement()) {

    //  t1 = clock();

    data->readNextFrame();
    if (data->getNewframe()->empty()) {
      cout <<data->getCameraName()<< " frame mal chopée\n";
      pthread_exit(NULL);
    }

    //partie traitement des images

    if (data->presenceMouvement()) {
      //créer et ouvrir le fichier video si il n'est pas ouvert
      if (! data->getWriter()->isOpened()) {
        pthread_t * thread = data->getThread(data->getCompteurThread());//-----------a revoir(on garde pas le thread)-------------------------------------zone sensible peu etre memoryleak ou erreur de seg
        if(! pthread_create(thread,NULL,TraitementVideo::writeThread,data) == 0 ){
          cout<<"problème de création de thread\n";
        }
        data->setCompteurThread((data->getCompteurThread()+1) % 2);

      }

      //ajout du contenue du buffer et de l'image du mouvement

      data->flushBuffer();
      data->getBuffer()->clearBuffer();
      cout<<"buffer is empty : "<< data->getBuffer()->isEmpty()<<"\n";
      //ajouter l'image
      data->getWriteQueue()->getQueue()->push(data->getNewframe()->clone());

    }



    else {
      //non mouvement
      //ajout de newframe au buffer

      data->getBuffer()->addMat(data->getNewframe()->clone());
      //cas pour la fermeture du fichier video
      if (data->getBuffer()->isFull() && data->getWriter()->isOpened()) {
        data->flushBuffer();
        data->getWriteQueue()->setContinueWrite(false);
      }
    }

    /*  t2 = clock();
    temps = (float) (t2-t1)/ CLOCKS_PER_SEC;
    printf("temps d'exec = %f \n", temps);*/
  }
  //si on attendait les 2 secondes de fin : on envoie ce qu'il y a dans le buffer
  if(data->getWriteQueue()->getContinueWrite()){
    data->flushBuffer();
  }

  while(data->getWriter()->isOpened()){
    // attend la fermeture du writer = fin du thread d'écriture
  }
  pthread_exit(NULL);

}


void TraitementVideo::toToWrite(queue<Mat> temp){
  while(! temp.empty()){
    this->writeQueue.getQueue()->push(temp.front());
    temp.pop();
  }
}

void *TraitementVideo::writeThread(void * arg){

  TraitementVideo * data = (TraitementVideo *) arg;

  data->getWriteQueue()->setContinueWrite(true);

  while(data->getWriter()->isOpened()){
  }
  time_t tmm = time(0);
  tm* now = localtime(&tmm);
  std::stringstream ss;
  ss <<data->getCameraName()<<"/"<< now->tm_mday << "-" << now->tm_mon+1 << "-" << now->tm_year+1900 << ":" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec << ".avi";
  //faire fonction interne open  avec nomfichier en param
  data->getWriter()->open(ss.str(),data->getCodec(),data->getFps(),data->getSize(),true);

  while(data->getWriteQueue()->getContinueWrite()){
    if(! data->getWriteQueue()->getQueue()->empty()){
      data->getWriter()->write(data->getWriteQueue()->getQueue()->front());
      data->getWriteQueue()->getQueue()->pop();
    }
  }
  data->getWriteQueue()->mutexBlock();
  queue<Mat> temp = data->getWriteQueue()->duplicateQueue();
  data->getWriteQueue()->mutexOpen();
  while(! temp.empty()){
    data->getWriter()->write(temp.front());
    temp.pop();
  }
  data->getWriter()->release();
  cout<<"fin thread écriture\n";
  pthread_exit(NULL);
}



void TraitementVideo::stop(){
  this->continueTraitement = false;
}

void TraitementVideo::initstop(){
  this->continueTraitement=true;
}

 // récupère l'ip dans l'url d'accès à la caméra
string TraitementVideo::getIp(){

    string protocol = this->url.substr(0, 4);
    string ip;

    if (protocol == "rtsp"){
        int pos1 = url.find("@") + 1;
        int pos2 = url.find_last_of(":");
        ip = url.substr(pos1, pos2-pos1);

    }
    else if(protocol == "http"){
        int pos1 = url.find_last_of("/") + 1;
        int pos2 = url.find_last_of(":");
        ip = url.substr(pos1, pos2-pos1);
    }
    else{
      ip = url;
      cout<<"protocole inconnue\n";
    }
    return ip;

}

bool TraitementVideo::pingIp(string ipAdress){  //retourne un boolean : true si l'adresse est accessible, false sinon
    int ping = system(("ping " + ipAdress).c_str());
    if (ping == 0){
      return true;
    }
    else{
      return false;
    }
}

void TraitementVideo::readNextFrame(){
  this->newframe.copyTo(this->oldframe);
  this->capture.read(this->newframe);
}








VideoCapture* TraitementVideo::getCapture(){
  return &this->capture;
}
void TraitementVideo::setCapture(VideoCapture capture){
  this->capture.release();
  this->capture = capture;
}


VideoWriter* TraitementVideo::getWriter(){
  return &this->writer;
}

void TraitementVideo::setWriter(VideoWriter writer){
  this->writer.release();
  this->writer = writer;
}


Buffer* TraitementVideo::getBuffer(){
  return &this->buffer;
}
void TraitementVideo::setBuffer(Buffer buffer){
  // detruire le buffer
  this->buffer = buffer;
}


Mat* TraitementVideo::getOldframe(){
  return &this->oldframe;
}
void TraitementVideo::setOldframe(Mat oldframe){
  this->oldframe.release();
  this->oldframe = oldframe;
}
void TraitementVideo::setOldframeCopy(Mat oldframe){
  this->oldframe.release();
  this->oldframe = oldframe.clone();
}


Mat* TraitementVideo::getNewframe(){
  return &this->newframe;
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


String TraitementVideo::getUrl(){
  return this->url;
}
//réinitialise tous les paramètres précédament assigné sauf le nom le codec, le seuil et la sensibilité
void TraitementVideo::setUrl(String url){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url = url;
  if(this->pingIp(this->getIp())){
    Mat imgex;
    this->capture.open(url);
    this->capture.read(imgex);

    this->setSize(imgex.cols,imgex.rows);
    double temp = this->capture.get(CAP_PROP_FPS);
    // on regarde si les fps sont valide car la fonction peut bug
    if(temp>120){
      temp =20;
    }
    this->setFps(temp);
  }
  else{
    cout<<"ip non reconnue\n";
  }
}


String TraitementVideo::getCameraName(){
  return this->cameraname;
}
void TraitementVideo::setCameraName(String cameraname){
    this->cameraname = cameraname;
}


Size TraitementVideo::getSize(){
  return this->size;
}
void TraitementVideo::setSize(Size size){
  this->size = size;
}
void TraitementVideo::setSize(int length, int height){
  this->size.height = height;
  this->size.width = length;
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
  return this->codec;
}
//le codec voulu est récupéré grace à la fonction static opencv VideoWriter::fourcc('X', 'X', 'X', 'X')
void TraitementVideo::setCodec(int codec){
  this->codec = codec;
}

int TraitementVideo::getCompteurThread(){
  return this->compteurThread;
}
void TraitementVideo::setCompteurThread(int i){
  this->compteurThread=i;
}

bool TraitementVideo::getContinueTraitement(){
  return this->continueTraitement;
}

pthread_t * TraitementVideo::getThread(int i){
  return &this->thread[i];
}

ToWrite* TraitementVideo::getWriteQueue(){
  return &this->writeQueue;
}




bool TraitementVideo::isCaptureOpened(){
  return this->capture.isOpened();
}
bool TraitementVideo::isWriterOpened(){
  return this->writer.isOpened();
}

bool TraitementVideo::writerOpen(string nomfichier){
  return this->writer.open(nomfichier,this->codec,this->fps,this->size,true);
}




// class toWrite


ToWrite::ToWrite(){
  this->mutex = PTHREAD_MUTEX_INITIALIZER;
}
ToWrite::~ToWrite(){
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

queue<Mat>* ToWrite::getQueue(){
  return &this->imgToWrite;
}
queue<Mat> ToWrite::duplicateQueue(){
  return this->imgToWrite;
}
