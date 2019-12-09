#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "buffer.hpp"



using namespace std;
using namespace cv;




//faire une fonction qui lit un ficher pour avoir l'ip
//faire une fonction qui récupère les valeurs de traitement depuis un fichier

//potentiellement trop lourde -> regarder pour faire une fonction que resize les images avant mais moins précit
//deux images en paramètre
bool presenceMouvement(Mat,Mat);

//même fonction qu'au dessus mais possibilité change la sensibilité de détection
bool presenceMouvement(Mat,Mat,int,double);

//insère dans la video(si ouverte) les images contenues dans le buffer de la plus vielle à la plus récente
void flushBuffer(cv::VideoWriter writer, Buffer* buffer);


int main(int argc, char* argv[]){

  if(argc != 3){
    cout<<"Le nombre de paramètre n'est pas bon\n";
    return -1;
  }


  //char ip[]="rtsp://admin:ouidenis7@192.168.0.64:554";
  //char ip[]="rtsp://Flavien:Ledeux57@192.168.1.150:88/videoMain";
  char* ip = argv[1];

  VideoCapture cap;
  VideoWriter writ;

  // faire un ping voir si la cam est up
  cap.open(ip);

  if (!cap.isOpened()) {
    //regarde si le stream mal ouvert
    cout<<"cam mal chopée\n";
    return -1;
  }

  // image actuelle
  Mat newframe;
  // image de reference
  Mat oldframe;

  //prend la première image -> pas de traitement nécéssaire
  cap.read(oldframe);
  //valeur fixe : provisoire -> regarder si CAP_PROP_FPS bug avec d'autres cams
  double fps =atof(argv[2]);
  if(fps==0){
    cout<<"valeur des fps incorrect\n";
    return -1;
  }
  //cap.get(CAP_PROP_FPS);

  // encodage de la video
  int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  //taille de l'image
  Size size(oldframe.cols,oldframe.rows);

  Buffer* buffer = new Buffer((int)fps*2);


  // pour voir le temps d'exec
  clock_t t1, t2;
  float temps;



  while(1) {

  //  t1 = clock();

    cap.read(newframe);

    if (newframe.empty()) {
      cout << "frame mal chopée\n";
      return -1;
    }

    //partie traitement des images

    if (presenceMouvement(oldframe,newframe)) {
        //créer et ouvrir le fichier video si il n'est pas ouvert
        if (! writ.isOpened()) {
            //je met la partie du titre (date+heure+.format)
            // le nom du fichier sera la date et l'heure
            time_t tmm = time(0);
            tm* now = localtime(&tmm);
            std::stringstream ss;
            ss << now->tm_mday << "-" << now->tm_mon+1 << "-" << now->tm_year+1900 << ":" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec << ".avi";
            string nomfichier = ss.str();

            writ.open(nomfichier,codec,fps,size,true);
        }

        //ajout du contenue du buffer et de l'image du mouvement
        // + vérification si le writer est ouvert pour plus de sécurité
        if(writ.isOpened()){
          flushBuffer(writ, buffer);
          buffer->clearBuffer();
          //ajouter l'image
          writ.write(newframe);
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


bool presenceMouvement(Mat input1,Mat input2){
  return presenceMouvement(input1,input2,50,0.1);//valeur par défaut si vide
}

//peut etre trop goumand niveau ressource- a test sur raspberry puis regarder solutions
bool presenceMouvement(Mat input1, Mat input2, int seuil, double pourcentage_meme) {

  Mat image_diff, image_diff_gris, image_binaire;

  //différence absolue des 2 images en input
  absdiff(input1 , input2  , image_diff);

  //met l'image de différence en noir et blanc+ nuances de gris
  cvtColor( image_diff, image_diff_gris, COLOR_BGR2GRAY );

  //met l'image en noir et banc en format binaire noir/blanc ajuster au seuil de différence
  threshold(image_diff_gris,image_binaire,seuil,255,0);

  //donne le pourcentage de pixels blanc
  double pourcentage = (double)countNonZero(image_binaire)/(image_binaire.rows*image_binaire.cols);

  return pourcentage>=pourcentage_meme;
}

void flushBuffer(cv::VideoWriter writer, Buffer* buffer){

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
