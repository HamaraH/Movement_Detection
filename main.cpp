#include "traitementvideo.hpp"
#include <stdio.h>
#include <iostream>

int main(){
  TraitementVideo t("rtsp://Flavien:Ledeux57@192.168.1.150:88/videoMain", "test1");
  cout<<"est ce que c'est bon\n";
  pthread_t thread;
  pthread_create(&thread,NULL,&TraitementVideo::traitement,&t);
  int d;
  printf("entrer un nombre pour arreter\n");
  scanf("%d",&d);
  t.stop();
  pthread_join(thread, NULL);
  exit(0);
}
