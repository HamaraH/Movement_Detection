#include <stdio.h>
#include <stdlib.h>
using namespace std;

char* getIpAdress(String path){
    	FILE* f = null;
	f = fopen(path, "r");
   	 if(f != null){
		char ip[15] = "";
		fgets(ip, 15, f); 
		printf("Caméra trouvée");
       		return ip;
    	}else{
        	printf("Impossible d'accéder aux adresses IP des caméras");
        	return null;
    	}
	fclose(f);
}
