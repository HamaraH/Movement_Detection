#include <string>
#include <fstream>
#include<iostream>
#include <vector>
using namespace std;

vector<string> getIpAdress(string path){   //path = chemin du fichier 
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

void print_vector(std::vector<string> &v){ //fonction qui affiche un vecteur, je l'ai utilisée pour tester je la laisse au cas ou
    for (int i = 0; i < v.size(); i++) {
	std::cout << v.at(i) << endl;
    }
}

int main(){
vector<string> ipList = getIpAdress("test.txt");
print_vector(ipList);
}
