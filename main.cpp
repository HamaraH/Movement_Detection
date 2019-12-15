#include "traitementvideo.hpp"
#include <stdio.h>

int main(){
  TraitementVideo t("rtsp://Flavien:Ledeux57@192.168.1.150:88/videoMain", "test1");
  pthread_t thread;
  pthread_create(&thread,NULL,TraitementVideo::traitement,&t);
  int d;
  printf("entrer un nombre pour arreter\n");
  scanf("%d",&d);
  t.stop();
  pthread_join(thread, NULL);
  return 0;
}
