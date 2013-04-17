#ifndef DESCRIPTORES_H
#define DESCRIPTORES_H

#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "image.h"
#include "SCM.h"
#include "filtroGabor.h"

class MomentosSinHistograma{

private:
    Image* imagen;

public:
    double * descriptores;

    int numDescriptores;

    MomentosSinHistograma(Image *imagen){
        this->imagen = imagen;
        this->descriptores = 0;
        this->numDescriptores = 0;

    }

    void calcDescriptores(){

        this->numDescriptores = 3;
        descriptores = new double[numDescriptores];

        double mean = 0;
        double stddev = 0;
        double variance = 0;

        int sizeImg = imagen->rows * imagen->cols;

        int i,j;
        for (i = 0; i < imagen->rows; ++i) {
            for (j = 0; j < imagen->cols; ++j)
                mean += imagen->pixelMat[i][j].canal[0];
        }

        mean /= sizeImg;

        for (i = 0; i < imagen->rows; ++i) {
            for (j = 0; j < imagen->cols; ++j)
                variance += (imagen->pixelMat[i][j].canal[0] - mean)*(imagen->pixelMat[i][j].canal[0] - mean);
        }

        variance /= sizeImg;

        stddev = sqrt(variance);

        descriptores[0] = mean;
        descriptores[1] = stddev;
        descriptores[2] = variance;
    }

    void showDescriptores(){
        cout<<"mean = "<<descriptores[0]<<endl;
        cout<<"stddev = "<<descriptores[1]<<endl;
        cout<<"variance = "<<descriptores[2]<<endl;
    }

    ~MomentosSinHistograma(){
        imagen->~Image();
        delete[]imagen;
        delete[]descriptores;
    }
};


class Histograma{

private:
    Image* imagen;

    int sizeImg;

public:

    double * descriptores;
    int numDescriptores;

    int range[2];

    int*vectorDensidadProb;
    int sizeVec;

    Histograma(Image *imagen, int rangoInf, int rangoSup){

        this->imagen = imagen;
        this->sizeImg = imagen->rows*imagen->cols;

        this->sizeVec = rangoSup-rangoInf;

        range[0] = rangoInf;
        range[1] = rangoSup;

        //Memoria
        vectorDensidadProb = new int[rangoSup-rangoInf+1];

    }

    void crearVectDensidadProb(int numCanal){
        int i,j;
        for (i = 0; i < sizeVec; ++i)
            vectorDensidadProb[i] = 0;

        for (i = 0; i < imagen->rows; ++i) {
            for (j = 0; j < imagen->cols; ++j)
                vectorDensidadProb[(int)imagen->pixelMat[i][j].canal[numCanal]]++;
        }
    }

    void calcDescriptores(){

        numDescriptores = 4;
        descriptores = new double[numDescriptores];


        //Color moments a partir del histograma
        double mean = 0;
        double stddev = 0;
        double variance = 0;

        double entropy = 0;

        int i;
        for ( i = range[0]; i < range[1]; ++i){
            mean += i*vectorDensidadProb[i];
            if (vectorDensidadProb[i] != 0)
                entropy -= vectorDensidadProb[i] * log((double)vectorDensidadProb[i]);
        }

        mean /= sizeImg;

        for ( i = range[0]; i < range[1]; ++i)
            variance += vectorDensidadProb[i] * (i - mean)*(i - mean);

        variance /= sizeImg;

        stddev = sqrt(variance);

        descriptores[0] = mean;
        descriptores[1] = stddev;
        descriptores[2] = variance;
        descriptores[3] = entropy;
    }

    void showVectorDensidadProb(){
        for (int i = range[0]; i < range[1]; ++i)
            cout<<vectorDensidadProb[i]<<"  ";
        cout<<endl<<endl;
    }

    void showDescriptores(){
        cout<<"mean = "<<descriptores[0]<<endl;
        cout<<"stddev = "<<descriptores[1]<<endl;
        cout<<"variance = "<<descriptores[2]<<endl;
        cout<<"entropy = "<<descriptores[3]<<endl<<endl;
    }

    void mostrarHistograma(Mat imagen, Mat histograma){

        int histSize = range[2]; //Eje x

        const float* histRange = { (float*)range };
        int tamVentana = 512;

        Mat histogramaBGR( histSize, histSize, CV_8UC3, Scalar( 0,0,0) );

        calcHist( &imagen, 1, 0, Mat(), histograma, 1, &histSize, &histRange, true, false );

        normalize(histograma, histograma, 0, histSize, NORM_MINMAX, -1, Mat() );

        for( int i = 1; i < histSize; ++i ){
            line( histogramaBGR, Point( tamVentana*(i-1), tamVentana - cvRound(histograma.at<float>(i-1)) ) ,
                Point( tamVentana*(i), tamVentana - cvRound(histograma.at<float>(i)) ),
                Scalar( 255, 0, 0), 2, 8, 0  );
        }

    }

    ~Histograma(){
        imagen->~Image();
        delete[]imagen;
        delete[]descriptores;
        delete[]vectorDensidadProb;
    }
};


class Descriptores {

public:

    Image* imagen;

    double * descriptores_MomSinH;
    double * descriptores_Hist;
    double * descriptores_SCM;
    double * descriptores_Gabor;

    MomentosSinHistograma *momentosSinH;
    Histograma *histograma;
    SCM *matrizSCM;
    FiltroGabor *filtroGabor;

    Descriptores();
    Descriptores(Image * imagen);

    void iniciarMomentosSinH();
    void iniciarHistograma(int rangoInf, int rangoSup);
    void iniciarSCM(int numNiveles);

    void iniciarGaborFilter(int kernelSize);

    void calcDescriptoresMomentos();
    void calcDescriptoresHistograma(int numCanal, int rangoInf, int rangoSup);
    void calcDescriptoresSCM(int* offsets, int distance, int direction);
    void calcDescriptoresGabor(float wavelength, float orientation, float phaseoffset, float gaussvar, float aspectratio);

    void setImagen(Image * imagen);



    ~Descriptores();

};



#endif // DESCRIPTORES_H

