#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include "buffer.hpp"

using namespace std;
using namespace cv;


//faire une fonction qui lit un ficher pour le flux video
//faire une fonction qui récupère les valeurs de traitement depuis un fichier txt
bool presence_mouvement(Mat,Mat,int,double);
bool presence_mouvement(Mat,Mat);
// regarder pour faire une fonction qui resize les images pour r"duire le temps de traitement si besoin

int main(int argc, char* argv[]){

/*char ip[200];
if(argc>1)
  ip = getConfigurationfromfile(argv[1]);
else{
  cout<<"saisir le chemin(200 caractère max)\n";
  char* path[200];
  scanf("%s",&path);
  ip = getConfigurationfromfile(path);
}*/
// partie capture du flux -> traitement de l'image a intégrer dedans

  char ip[]="rtsp://Flavien:Ledeux57@192.168.1.150:88/videoMain";

  VideoCapture cap;
	VideoWriter writ;
  // faire un ping voir si la cam est up
  cap.open(ip);


  if(!cap.isOpened()){//regarde si le stream mal ouvert
    cout<<"cam mal chopée\n";
    return -1;
  }

  cout << "Start grabbing" << "\n";

  Mat oldframe;//= une image
  Mat newframe;//= une image

  cap.read(oldframe);//prend la première image -> pas de traitement nécéssaire

  double fps = cap.get(CAP_PROP_FPS);
  int codec = cap.get(CAP_PROP_FOURCC); // encodage des images
	Size size(newframe.cols,newframe.rows);//taille de l'image

  Mat* tab;

  clock_t t1, t2;// pour voir le temps d'exec
  float temps;
  while(1){
    t1 = clock();
    cap.read(newframe);

    if(newframe.empty()){
      cout<<"frame mal chopée\n";
      return -1;
    }

    //partie traitement des images
printf("%d\n",presence_mouvement(oldframe,newframe));
/*
    //mouvement
    if(presence_mouvement(oldframe,newframe)){

			if(!writ.isOpened()){
				//ourir le writ
        time_t tmm = time(0);
				char* nomfichier = ctime(&tmm);// le nom du fichier sera la date et l'heure
				//fps a recup de l'input
				wit.open(nomfichier,codec,fps,size,true);
			}
			//ajouter le buffer si nonvide
			if(!buffer.is_empty()){
				tab = buffer.get_buffer();//obtien le buffer, il se peut que certaine cases du tab ne sois pas remplient
				for(int i=0;i<buffer.get_buffer_size();i++){
          if(tab[i]==NULL)
            break;
          writ.write(tab[i]);
			}
      buffer.clear_buffer();
			//ajouter l'image
			writ.write(newframe);
		}





    //non mouvement
    else{
			//ajouter newframe au buffer l'array
			buffer.set_last_mat(newframe);
			if(buffer.is_full()&&writ.isOpened()){//fonction du buffer pas encore fait + type du buffer pas encore créer
				//importe le tableau du buffer
        tab=buffer.get_buffer();
				for(int i=0;i<buffer.get_buffer_size();i++)//size(buffer) pas encore def
					writ.write(tab[i]);

				writ.release();
			}
		}


*/

		newframe=oldframe;
    t2 = clock();
    temps = (float) (t2-t1)/ CLOCKS_PER_SEC;
    printf("temps d'exec = %f \n", temps);
  }

/*//test unitaire de detection de mouvement -> à agrandire
	//variables image nécéssaire à la transformation
	Mat image1= imread("index.jpg", CV_LOAD_IMAGE_COLOR);
	Mat image2= imread("index2.jpg", CV_LOAD_IMAGE_COLOR);

  //cout<<"test false :"<<endln;
  //cout<<presence_mouvement(image1,image1)<<endln;

  cout<<"test true :"<<"\n";
  cout<<presence_mouvement(image1,image2)<<"\n";
	sleep(60);
*/
  return 0;

}

bool presence_mouvement(Mat input1,Mat input2){
  return presence_mouvement(input1,input2,25,0.1);//valeur par défaut si vide
}


bool presence_mouvement(Mat input1,Mat input2,int seuil,double pourcentage_meme){//peut etre trop goumand niveau ressource- a test sur raspberry puis regarder solutions

  Mat image_diff;
  Mat image_diff_gris;
  Mat image_binaire;

//différence absolue des 2 images en input
  absdiff(input1 , input2  , image_diff);

//met l'image de différence en noir et blanc+ nuances de gris
  cvtColor( image_diff, image_diff_gris, COLOR_BGR2GRAY );

//met l'image en noir et banc en format binaire noir/blanc ajuster au seuil de différence
  threshold(image_diff_gris,image_binaire,seuil,255,0);

 /*//affiche les étapes de transformation des images
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

  if(pourcentage>=pourcentage_meme)
    return true;
  else
    return false;
}