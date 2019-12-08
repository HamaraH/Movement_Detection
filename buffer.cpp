#include <opencv2/opencv.hpp>

#include "buffer.hpp"

Buffer::Buffer(int fps){  //Constructeur
    buffer= new cv::Mat[fps];
    size = fps;
    last_index = 0;
}

Buffer::Buffer(){  //Constructeur
    size = 30;
    buffer = new cv::Mat[30];
    last_index = 0;
}

Buffer::~Buffer(){
    delete[] buffer;
}

//Recupere la taille du buffer
int Buffer::get_buffer_size(){
    return size;
}

int Buffer::get_last_index(){
    return last_index;
}

cv::Mat* Buffer::get_buffer(){    //Obtenir le buffer en entier
    cv::Mat* tab;
    tab = new cv::Mat[size];

    /*cv::Mat* start = get_part_buffer(this->last_index,this->size-1);
    cv::Mat* end = get_part_buffer(0,this->last_index-1);*/

    int j = 0;

    for( int i = last_index ; i<=size-1 ; i++ ) {
        tab[j] = buffer[i];
        j++;
    }

    for (int i = 0 ; i <= last_index - 1 ; i++ ) {
        tab[j] = buffer[i];
        j++;
    }

    return tab;
}

//Remettre le buffer à 0
void Buffer::clear_buffer(){
    for ( int i = 0 ; i < size ; i++ ){
        buffer[i].release();
// !!!       buffer[i] = NULL;
    }
    last_index = 0;
}

//Verifier si le buffer est rempli
bool Buffer::is_full(){
    if ( (bool) buffer[ size-1 ].empty()  )
        return  true;
    else
        return false;
}

//Verifier si le buffer est rempli
bool Buffer::is_empty(){
    if ((bool) buffer[ 0 ].empty())
         return  true;
    else
         return false;
}

cv::Mat Buffer::get_last_mat(){
    return buffer[last_index-1];
}

void Buffer::set_last_mat(cv::Mat image){
    buffer[last_index].release();
    buffer[last_index] = image;
    if (last_index == size-1)
        last_index = 0;
    else
        last_index++;
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
