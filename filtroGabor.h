
#ifndef FILTROGABOR_H
#define FILTROGABOR_H

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


class FiltroGabor{

private:

    Mat *imagen;

    Mat imagen_f;

    Mat kernel;



    int kernelSize;

public:

    Mat kernelMostrar;

    FiltroGabor(Mat &imagen, int kernelSize){
        this->imagen = &imagen;

        imagen.convertTo(imagen_f, CV_32F, 1.0/255, 0);

        //Impar
        if (!kernelSize%2)
            kernelSize++;

        this->kernelSize = kernelSize;

        //KERNEL
        kernel = Mat(kernelSize, kernelSize, CV_32F);
    }

    void filtrarGabor(Mat &imagenRes){

        //Filtrado a la imagen
        filter2D(imagen_f, imagenRes, CV_32F, kernel);

    }

    float valorFiltroGabor(int x,
                       int y,
                       float lambda,
                       float theta,
                       float phi,
                       float sigma,
                       float gamma)
    {

        //double escala = 2.0/(kernelSize-1);
        double escala = 1;


        float xx =  x * escala*cos(theta) + y * escala*sin(theta);
        float yy = -x * escala*sin(theta) + y * escala*cos(theta);

        float envelopeVal = (float)exp( - ( (xx*xx + gamma*gamma* yy*yy) / (2.0f * sigma*sigma) ) );

        float carrierVal = cos( 2.0f * (float)CV_PI * xx / lambda + phi);

        float g = envelopeVal * carrierVal;

        return g;
    }

    void crearKernel(float wavelength, float orientation, float phaseoffset, float gaussvar, float aspectratio){

        int my = (kernel.rows-1)/2;
        int mx = (kernel.cols-1)/2;

        float valor;

        int y,x;

        //Verificar Valores

        /////////////// wavelength ///////////////
        if(!(wavelength >= 2))
            cout<<"wavelength No Valido: Fuera de rango";

        if(wavelength == 2 && (phaseoffset == -90 || phaseoffset == -90) )
            cout<<"wavelength No Valido: wavelength = 2 y phaseoffset = -90 o 90";

        if(wavelength > imagen->rows/5 || wavelength > imagen->cols/5)
            cout<<"wavelength No Valido: Posibles efectos no deseados en los bordes de la imagen";

        /////////////// orientation ///////////////

        if(!(orientation >= 0 && orientation <= 360))
            cout<<"orientation No Valido: Fuera de rango";

        orientation = orientation*CV_PI/180; //convertir a grados

        /////////////// phaseoffset ///////////////

        if(!(phaseoffset >= -180 && phaseoffset <= 180))
            cout<<"phaseoffset No Valido: Fuera de rango";

        phaseoffset = phaseoffset*CV_PI/180; //convertir a grados

        /////////////// aspectratio ///////////////

        if(!(aspectratio > 0))
            cout<<"aspectratio No Valido: Fuera de rango";


        for (y=-my; y<=my; ++y){
            for (x=-mx; x<=mx; ++x){

                valor = valorFiltroGabor(x,y, wavelength, orientation, phaseoffset, gaussvar, aspectratio);

                //row, col
                kernel.at<float>(my+y, mx+x) = valor;

            }
        }

    }

    void mostrarKernel(){

        kernelMostrar = Mat(200, 200, CV_32F);

        resize(kernel, kernelMostrar, kernelMostrar.size());

        kernelMostrar /= 2.;
        kernelMostrar += 0.5;

        kernelMostrar.convertTo(kernelMostrar, CV_8U,255,0);

        //namedWindow("KERNEL", 1);
        //imshow("KERNEL", kernelMostrar);

        //waitKey();
    }

    ~FiltroGabor(){

    }

};


#endif // FILTROGABOR_H
