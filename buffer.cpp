#include <opencv2/opencv.hpp>

#include "buffer.hpp"

Buffer::Buffer(int fps){  //Constructeur
    buffer= new cv::Mat[fps];
    size = fps;
    head = 0;
}

Buffer::Buffer(){  //Constructeur
    size = 30;
    buffer = new cv::Mat[30];
    head = 0;
}

Buffer::~Buffer(){
    delete[] buffer;
}

//Recupere la taille du buffer
int Buffer::getSize(){
    return size;
}

int Buffer::getHead(){
    return head;
}

cv::Mat* Buffer::getBuffer(){    //Obtenir le buffer en entier
    cv::Mat* tab;
    tab = new cv::Mat[size];

    /*cv::Mat* start = get_part_buffer(this->head,this->size-1);
    cv::Mat* end = get_part_buffer(0,this->head-1);*/

    int j = 0;

    for( int i = head ; i<=size-1 ; i++ ) {
        tab[j] = buffer[i];
        j++;
    }

    for (int i = 0 ; i <= head - 1 ; i++ ) {
        tab[j] = buffer[i];
        j++;
    }

    return tab;
}

//Remettre le buffer à 0
void Buffer::clearBuffer(){
    for ( int i = 0 ; i < size ; i++ ){
        buffer[i].release();
// !!!       buffer[i] = NULL;
    }
    head = 0;
}

//Verifier si le buffer est rempli
bool Buffer::isFull(){
    return ! (bool) buffer[ size-1 ].empty();
}

//Verifier si le buffer est rempli
bool Buffer::isEmpty(){
    return (bool) buffer[ 0 ].empty())

}

cv::Mat Buffer::getLastMat(){
    return buffer[head-1%size];
}

void Buffer::addMat(cv::Mat image){
    buffer[head].release();
    buffer[head] = image;
    if (head == size-1)
        head = 0;
    else
        head++;
}


 /*
cv::Mat* Buffer::get_part_buffer(int a, int b){
    cv::Mat resultat[];
    resultat = (cv::Mat*) malloc (((b-a)+1) * sizeof(cv::Mat));
    int j = 0;
    for(int i = a ; i<=b ; i++){
      resultat[j] = this->buffer[i];
      j++;
    }
    return resultat;
}
*/
