#include <iostream>
#include <stdio.h>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;


//faire une fonction qui compare les 2 images d'entré
//faire une fonction qui lit un ficher pour le flux video
//faire une fonction qui récupère les valeurs de traitement depuis un fichier txt

int main(int argc, char* argv[]){


/* partie capture du flux -> traitement de l'image a intégrer dedans

  VideoCapture cap;
  cap.open("http://192.168.0.2:8080/cam");

  if(!cap.isOpened()){//regarde si le stream mal ouvert
    cout<<"cam mal chopée\n";
    return -1;
  }

  cout << "Start grabbing" << endl

  Mat oldframe;//= une image
  Mat newframe;//= une image

  cap.read(oldframe);/prend la première image -> pas de traitement nécéssaire

  while(1){

    cap.read(newframe);
    cout<<"cam chopée\n";

    if(newframe.empty()){
      cout<<"frame mal chopée\n";
      return -1;
    }

    //partie traitement des images


  }*/


//variables image nécéssaire à la transformation -> voir pour réduire?
	Mat image1= imread("index.jpg", CV_LOAD_IMAGE_COLOR);
	Mat image2= imread("index2.jpg", CV_LOAD_IMAGE_COLOR);
	Mat image_diff;
  Mat image_diff_gris;
  Mat image_binaire;


// a regarde pour les prendre d'un fichier ou les saisir manuellement
  const int seuille=25;//valeur seuille des pixel à la couleur blanche  -->a continuer de test avec cas réel
  const double pourcentage_meme = 0.1;//pourcentage de similite nécéssaire pour juger si il y a mouvement



//différence absolue des 2 images en input
 	cv::absdiff(image1 , image2  , image_diff);

//met l'image de différence en noir et blanc+ nuances de gris
  cvtColor( image_diff, image_diff_gris, CV_BGR2GRAY );

//met l'image en noir et banc en format binaire noir/blanc ajuster au seuille de différence
	threshold(image_diff_gris,image_binaire,value,255,0);

/* affiche les étapes de transformation des images
  imshow("diff",image_diff);
	imshow("gris",image_diff_gris);
  imshow("binaire",image_binaire);
*/

/* affiche le nombre de pixel blanc
  int nbblanc  = countNonZero(image_binaire);
  printf("%d",nbblanc);
*/


  double pourcentage = (double)countNonZero(image_binaire)/(image_binaire.rows*image_binaire.cols);//donne le pourcentage de pixels blanc

//printf("%lf\n",pourcentage);//affiche le pourcentage de différence



	waitKey(0);

  return 0;

}
