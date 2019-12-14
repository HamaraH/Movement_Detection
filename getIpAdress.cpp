#include <string>
#include <fstream>
#include<iostream>
#include <vector>
#include <cstdlib>
using namespace std;

vector<string> getAllAdresses(string path){   //path = chemin du fichier
    ifstream f(path.c_str());
    vector<string> ip; //contient les adresses ip
    string adress;
    if(f){ // si le chemin est bon
         int i =0;
         while(getline(f, adress)){ //on lit le fichier ligne par ligne
             ip.push_back(adress);
             i++;
         }
   }else{ // si le fichier ne s'ouvre pas : erreur
         cout << "Impossible d'acceder aux adresses IP des cameras" << endl;
    }
   f.close();
   return ip;

}

void printVector(std::vector<string> &v){ //fonction qui affiche un vecteur, je l'ai utilisée pour tester je la laisse au cas ou
    for (int i = 0; i < v.size(); i++) {
	std::cout << v.at(i) << endl;
    }
}

string getIp(string url){ // récupère l'ip dans l'url d'accès à la caméra
    string protocol = url.substr(0, 4);
    string ip;
    if(protocol == "rtsp"){
        int pos1 = url.find("@") + 1;
        int pos2 = url.find_last_of(":");
        ip = url.substr(pos1, pos2-pos1);
    }else if(protocol == "http"){
        int pos1 = url.find_last_of("/") + 1;
        int pos2 = url.find_last_of(":");
        ip = url.substr(pos1, pos2-pos1);
    }
    return ip;

}

bool pingIp(string ipAdress){  //retourne un boolean : true si l'adresse est accessible, false sinon
    int ping = system(("ping " + ipAdress).c_str());
    if(ping == 0){
        return true;
    }else{
        return false;
    }
}



int main(){
    vector<string> ipList = getAllAdresses("test.txt"); //on récupère les adresses ip présentes dans le fichier
    printVector(ipList);
    bool ping;
    for(int i = 0; i < ipList.size(); i++){
        ping = pingIp(getIp(ipList.at(i)));
        if(ping == true){
            cout << "adresse ip " + getIp(ipList.at(i)) + " valide" << endl;
        }else{
            cout << "adresse ip " + getIp(ipList.at(i)) + " invalide" << endl;
        }
    }
}
