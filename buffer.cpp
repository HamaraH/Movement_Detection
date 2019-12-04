#include <opencv2/opencv.hpp>
#include "buffer.hpp"

class Buffer{

private:

  cv::Mat buffer[];   //Stockage des images
  int size;       //Taille du tableau
  int last_index = 0; //Index de la dernière image stockée

public:

  Buffer(int size){  //Constructeur

    this->buffer = (cv::Mat*) malloc(size*sizeof(cv::Mat));

  }

  Buffer(){  //Constructeur

    this->size = 30;
    this->buffer = (cv::Mat*) malloc(30*sizeof(cv::Mat));

  }
  
  ~Buffer(){
    
    delete[] buffer;
    
  }
    
  int get_buffer_size(){   //Recupere la taille du buffer

    return this->size;

  }

  cv::Mat* get_buffer(){    //Obtenir le buffer en entier

    cv::Mat tab[];
    tab = (cv::Mat*) malloc(size*sizeof(cv::Mat));

    cv::Mat* start = get_part_buffer(this->last_index,this->size-1);
    cv::Mat* end = get_part_buffer(0,this->last_index-1);
    
    int j = 0;

    for( int i = this->last_index ; i<=size-1 ; i++ ){

      tab[j] = this->buffer[i];
      j++;

    }

    for( int i = 0 ; i<=this->last_index - 0 ; i++ ){

      tab[j] = this->buffer[i];
      j++;

    }

  }

  void clear_buffer(){  //Remettre le buffer à 0

    for ( int i = 0 ; i < this->size ; i++ ){

      this->buffer[i] = NULL;

    }

    this->last_index = 0;

  }

  bool is_full(){  //Verifier si le buffer est rempli

    if (this->buffer[ size-1 ] == NULL)

        return  true;

    else

      return false;

    }

  }

  bool is_empty(){  //Verifier si le buffer est rempli

    if (this->buffer[ 0 ] == NULL)

        return  true;

    else

      return false;

    }

  }

  cv::Mat get_last_mat(){

    return buffer[last_index-1];

  }

  void set_last_mat(cv::Mat image){

    this->buffer[last_index] = image;

    if (this->last_index == size-1)
      this->last_index = 0;

    else 

      this->last_index++;

  }


  cv::Mat* get_part_buffer(int a, int b){

    cv::Mat resultat[];
    resultat = (cv::Mat*) malloc (((b-a)+1) * sizeof(cv::Mat));
    int j = 0;

    for( int i = a ; i<=b ; i++){

      resultat[j] = this->buffer[i];
      j++;

    }

    return resultat;
  }

};
