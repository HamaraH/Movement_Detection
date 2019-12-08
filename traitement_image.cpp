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




//faire une fonction qui lit un ficher pour le flux video

//faire une fonction qui récupère les valeurs de traitement depuis un fichier txt

bool presence_mouvement(Mat,Mat,int,double);
bool presence_mouvement(Mat,Mat);




// regarder pour faire une fonction qui resize les images pour r"duire le temps de traitement si besoin
int main(int argc, char* argv[]){

  // partie capture du flux -> traitement de l'image a intégrer dedans
  //char ip[]="rtsp://admin:ouidenis7@192.168.0.64:554";
  char ip[]="rtsp://Flavien:Ledeux57@192.168.1.150:88/videoMain";

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
  double fps = 15;
  //cap.get(CAP_PROP_FPS);

  // encodage des images
  int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
  //taille de l'image
  Size size(oldframe.cols,oldframe.rows);

  Buffer* buffer = new Buffer((int)fps*2);

  Mat* tab;

  // pour voir le temps d'exec
  clock_t t1, t2;
  float temps;


  bool mouv;
  cout<<(int)fps*2<<"\n";
  while(1) {



  //  t1 = clock();


    cap.read(newframe);

    if (newframe.empty()) {
      cout << "frame mal chopée\n";
      return -1;
    }

    //partie traitement des images

    //printf("%d\n",presence_mouvement(oldframe,newframe));

    //mouvement

    mouv =presence_mouvement(oldframe,newframe);

    if (mouv) {
        if (! writ.isOpened()) {
            //ouvrir le writer

            time_t tmm = time(0);
            tm* now = localtime(&tmm);

            std::stringstream ss;

            //je met la partie du titre (date+heure+.format)
            // le nom du fichier sera la date et l'heure
            ss << now->tm_mday << "-" << now->tm_mon+1 << "-" << now->tm_year+1900 << ":" << now->tm_hour << "-" << now->tm_min << "-" << now->tm_sec << ".avi";

            //string nomfichier("output.avi");
            string nomfichier = ss.str();

            //fps a recup de l'input
            writ.open(nomfichier,codec,fps,size,true);
        }

        //ajouter le buffer si nonvide
        if(writ.isOpened()){
          if (! buffer->is_empty()) {

              //obtient le buffer, il se peut que certaine cases du tab ne sois pas remplient
              tab = buffer->get_buffer();
              for (int i=0;i<buffer->get_buffer_size();i++) {
                  if (tab[i].empty()) {
                      break;
                  }
                  writ.write(tab[i]);
              }
              delete[] tab;
              buffer->clear_buffer();
          }
          //ajouter l'image
          writ.write(newframe);
      }
    }



    else {
        //non mouvement

        //ajouter newframe au buffer l'array
        buffer->set_last_mat(newframe.clone());
        cout<<buffer->is_full();
        //fonction du buffer pas encore fait + type du buffer pas encore créer
        if (buffer->is_full() && writ.isOpened()) {
            //importe le tableau du buffer
            tab=buffer->get_buffer();

            for(int i=0;i<buffer->get_buffer_size();i++) {
                //size(buffer) pas encore def
                writ.write(tab[i]);
            }
            delete[] tab;
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

bool presence_mouvement(Mat input1,Mat input2){
  return presence_mouvement(input1,input2,50,0.1);//valeur par défaut si vide
}

//peut etre trop goumand niveau ressource- a test sur raspberry puis regarder solutions
bool presence_mouvement(Mat input1, Mat input2, int seuil, double pourcentage_meme) {

  Mat image_diff, image_diff_gris, image_binaire;
  //différence absolue des 2 images en input

  absdiff(input1 , input2  , image_diff);

  //met l'image de différence en noir et blanc+ nuances de gris

  cvtColor( image_diff, image_diff_gris, COLOR_BGR2GRAY );

  //met l'image en noir et banc en format binaire noir/blanc ajuster au seuil de différence

  threshold(image_diff_gris,image_binaire,seuil,255,0);

  //donne le pourcentage de pixels blanc
  double pourcentage = (double)countNonZero(image_binaire)/(image_binaire.rows*image_binaire.cols);
  //cout << pourcentage << "\n";

  image_diff.release();
  image_diff_gris.release();
  image_binaire.release();

  return pourcentage>=pourcentage_meme;
}
