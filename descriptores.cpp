#include "descriptores.h"

#include <iostream>
#include <fstream>

#include "imgOperaciones.h"

using namespace imgOperaciones;

using namespace std;

Descriptores::Descriptores(){
}

Descriptores::Descriptores(Image * imagen){
    this->imagen = imagen;
}

void Descriptores::setImagen(Image * imagen){
    this->imagen = imagen;
}

void Descriptores::iniciarMomentosSinH(){
    momentosSinH = new MomentosSinHistograma(imagen);
}

void Descriptores::iniciarHistograma(int rangoInf, int rangoSup){
    histograma = new Histograma(imagen, rangoInf, rangoSup);
}

void Descriptores::iniciarSCM(int numNiveles){
    matrizSCM = new SCM(imagen, numNiveles);
}

void Descriptores::iniciarGaborFilter(int kernelSize){
    filtroGabor = new FiltroGabor(imagen->imagenMat, kernelSize);
}

//1ER ORDEN
void Descriptores::calcDescriptoresMomentos(){

    //Color moments sin Histograma
    momentosSinH->calcDescriptores();

    //momentosSinH->showDescriptores();

    descriptores_MomSinH = momentosSinH->descriptores;
}

void Descriptores::calcDescriptoresHistograma(int numCanal, int rangoInf, int rangoSup){

    //Color moments de Histograma

    histograma->crearVectDensidadProb(numCanal);

    histograma->calcDescriptores();

    //histograma->showDescriptores();

    descriptores_Hist = histograma->descriptores;

}


//2DO ORDEN
void Descriptores::calcDescriptoresSCM(int* offsets, int distance, int direction){


    matrizSCM->createSCM(offsets, distance, direction);

    matrizSCM->normalizar();

    matrizSCM->calcDescriptores();

    //matrizSCM->showDescriptores();
    //matrizSCM.showFileSCM();

    descriptores_SCM = matrizSCM->descriptores;



    //matrizSCM->calcDescsVentanas(3, 3, offsets, distance, direction);
}

//Espectrales
void Descriptores::calcDescriptoresGabor(float wavelength, float orientation, float phaseoffset, float gaussvar, float aspectratio){

    //mostrarPropiedadesImg(imagen->imagenMat);

    filtroGabor->crearKernel(wavelength, orientation, phaseoffset, gaussvar, aspectratio);

    filtroGabor->mostrarKernel();

    //Guardar imagen filtrada en imagen
    filtroGabor->filtrarGabor(imagen->imagenMat);

    imagen->imagenMat.convertTo(imagen->imagenMat, CV_8U,255,0);

    //namedWindow("IMAGEN_FILTRADA GABOR", 1);
    //imshow("IMAGEN_FILTRADA GABOR", imagen->imagenMat);
    //waitKey();


    imagen->actualizarPixelMat_1C();

    //Calcular descriptores

    iniciarMomentosSinH();

    calcDescriptoresMomentos();

    descriptores_Gabor = descriptores_MomSinH;
}


Descriptores::~Descriptores(){


}
