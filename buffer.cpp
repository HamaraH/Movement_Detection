#include <opencv2/opencv.hpp>
#include <buffer.hpp>

class Buffer{

private:

  Mat buffer[];   //Stockage des images
  int size;       //Taille du tableau
  int last_index = 0; //Index de la dernière image stockée

public:

  Buffer(int size){  //Constructeur

    this.buffer = (Mat*) malloc(size*sizeof(Mat));

  }

  Buffer(){  //Constructeur

    this.size = 30;
    this.buffer = (Mat*) malloc(30*sizeof(Mat));

  }

  int get_buffer_size(){   //Recupere la taille du buffer

    return this.size;

  }

  Mat* get_buffer(){    //Obtenir le buffer en entier

    Mat tab[];
    tab = (Mat*) malloc(size*sizeof(Mat));

    Mat* start = get_part_buffer(this.last_index,this.size-1);
    Mat* end = get_part_buffer(0,this.last_index-1);
    
    int j = 0;

    for( int i = this.last_index ; i<=size-1 ; i++ ){

      tab[j] = this.buffer[i];
      j++;

    }

    for( int i = 0 ; i<=this.last_index - 0 ; i++ ){

      tab[j] = this.buffer[i];
      j++;

    }

  }

  void clear_buffer(){  //Remettre le buffer à 0

    for ( int i = 0 ; i < this.size ; i++ ){

      this.buffer[i] = NULL;

    }

    this.last_index = 0;

  }

  boolean is_full(){  //Verifier si le buffer est rempli

    if (this.buffer[ size-1 ] == NULL)

        return  true;

    else

      return false;

    }

  }

  boolean is_empty(){  //Verifier si le buffer est rempli

    if (this.buffer[ 0 ] == NULL)

        return  true;

    else

      return false;

    }

  }

  Mat get_last_mat(){

    return buffer[last_index-1];

  }

  void set_last_mat(Mat image){

    this.buffer[last_index] = image;

    if (this.last_index == size-1)
      this.last_index = 0;

    else 

      this.last_index++;

  }


  Mat* get_part_buffer(int a,b){

    Mat resultat[];
    resultat = (Mat*) malloc (((b-a)+1) * sizeof(Mat));
    int j = 0;

    for( int i = a ; i<=b ; i++){

      resultat[j] = this.buffer[i];
      j++;

    }

    return resultat;
  }

}
