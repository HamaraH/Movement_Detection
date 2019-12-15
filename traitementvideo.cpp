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

    this->cameraname = this.capture.getBackendName();
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
    this->cameraname = this.capture.getBackendName();
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

void * TraitementVideo::traitement(){

  if(tnis->cameraname.empty()){
    this->cameraname->"defaultname";
  }
  if(! this->capture.isOpened()){
    if(this->pingIp(this->getIp())){
    this->capture.open(this->ip);
    }
    else{
      cout<<"ip non reconnue\n";
      pthread_exit(NULL);
    }
    //si pas bon afficher erreur et return -1
  }

  if (! this->capture.isOpened()) {
    //regarde si le stream mal ouvert
    cout<<this->cameraname<<" cam mal chopée\n";
    pthread_exit(NULL);
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

  this->compteurThread=0;

  // pour voir le temps d'exec
  //clock_t t1, t2;
  //float temps;


  this->continueTraitement = true;
  while(this->continueTraitement) {

    //  t1 = clock();

    this->capture.read(this->newframe);

    if (this->newframe.empty()) {
      cout <<yhis->cameraname<< " frame mal chopée\n";
      pthread_exit(NULL);
    }

    //partie traitement des images

    if (this->presenceMouvement()) {
      //créer et ouvrir le fichier video si il n'est pas ouvert
      if (! this->writer.isOpened()) {
        if(! pthread_create(&this->thread[this->compteurThread],NULL,TraitementVideo::writeThread,&this)){
          cout<<"problème de création de thread\n";
        }
        //devoir tempo un coup
        while(! this->writeQueue.getContinueWrite()){
          //boucle d'attente le temps que compteur thread soit dupliqué
        }
        this->compteurThread = this->compteurThread++ % 2;

        //mettre la variable de la boucle a true
      }

      //ajout du contenue du buffer et de l'image du mouvement

      this->flushBuffer();
      this->buffer.clearBuffer();
      //ajouter l'image
      this->writeQueue.getQueue().push(this->newframe.clone());

    }



    else {
      //non mouvement

      //ajout de newframe au buffer
      this->buffer->addMat(this->newframe.clone());

      //cas pour la fermeture du fichier video
      if (this->buffer->isFull() && this->writer.isOpened()) {
        this->flushBuffer();
        this->writeQueue.setContinueWrite(false);
      }
    }

    this->oldframe.release();
    this->oldframe=this->newframe.clone();// regarder pour l'effacer: remplacer oldframe par la denrière image du buffer

    /*  t2 = clock();
    temps = (float) (t2-t1)/ CLOCKS_PER_SEC;
    printf("temps d'exec = %f \n", temps);*/
  }
  if(this->writeQueue.getContinueWrite()){
    this->flushBuffer();
  }
  pthread_join(this->thread[0],NULL);
  pthread_join(this->thread[1],NULL);
  pthread_exit(NULL);

}


void TraitementVideo::toToWrite(queue<Mat> temp){
  while(! temp.empty()){
    this->toWrite.push(temp.front());
    temp.pop();
  }
}

void * TraitementVideo::writeThread(void * arg){
  TraitementVideo * data = (TraitementVideo *) arg;
  int i = (data->compteurThread +1) % 2;
  data->writeQueue.setContinueWrite(true);
  pthread_join(data->thread[i],NULL);

  time_t tmm = time(0);
  tm* now = localtime(&tmm);
  std::stringstream ss;
  ss <<this->cameraname<<"/"<< now->tm_mday << "-" << now->tm_mon+1 << "-" << now->tm_year+1900 << ":" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec << ".avi";
  string nomfichier = ss.str();

  data->writer.open(nomfichier,data->codec,data->fps,data->size,true);


  while(data->continueWrite){
    if(! data->writeQueue.getQueue().empty()){
      data->writer.write(data->writeQueue.getQueue().front());
      data->writeQueue.getQueue().pop();
    }
  }
  data->writeQueue.mutexBlock();
  queue<Mat> temp = data->writeQueue.getQueue()
  data->writeQueue.mutexOpen();
  while(! temp.empty()){
    data->writer.write(temp.front());
    temp.pop();
  }
  data->writer.release();
  pthread_exit(NULL);
}



void TraitementVideo::stop(){
  this.continueTraitement = false;
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









VideoCapture TraitementVideo::getCapture(){
  return this->capture;
}
void TraitementVideo::setCapture(VideoCapture capture){
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


String TraitementVideo::getUrl(){
  return this->url;
}
//réinitialise tous les paramètres précédament assigné sauf le nom le codec, le seuil et la sensibilité
int TraitementVideo::setUrl(String url){
  if(this->capture.isOpened()){
    this->capture.release();
  }
  this->url = url;
  if(this->pingIp(this->getIpbrut())){
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
  }
  else{
    cout<<"ip non reconnue\n";
  }
}


String TraitementVideo::getCameraName(){
  return this->cameraname;
}
String TraitementVideo::setCameraName(String cameraname){
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

queue<Mat> ToWrite::getQueue(){
  return this->imgToWrite;
}
