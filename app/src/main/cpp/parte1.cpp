#include "parte1.h"
#include "zernike.cpp" // Incluir el archivo zernike.cpp
#include <android/log.h>
#include <android/asset_manager.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <cfloat>
#include <algorithm>
#include <numeric>
#include <cmath>

#define LOG_TAG "PROYECTO_VISION"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;
using namespace std;

vector<pair<vector<double>, vector<double>>> momentosHuZernikeCirculos;
vector<pair<vector<double>, vector<double>>> momentosHuZernikeCuadrados;
vector<pair<vector<double>, vector<double>>> momentosHuZernikeTriangulos;

void calcularMomentosHu(const Mat& imagen, double momentosHu[7]) {
    Moments momentos = moments(imagen, true);
    HuMoments(momentos, momentosHu);
}

double distanciaEuclidea(const vector<double>& momentos1, const vector<double>& momentos2) {
    double suma = 0;
    for (size_t i = 0; i < momentos1.size(); ++i) {
        suma += pow(momentos1[i] - momentos2[i], 2);
    }
    return sqrt(suma);
}

void rellenar(const Mat& imgBinaria, Mat& imgRelleno) {
    vector<vector<Point>> contornos;
    findContours(imgBinaria, contornos, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    imgRelleno = Mat::zeros(imgBinaria.size(), CV_8UC1);
    for (const auto& contorno : contornos) {
        drawContours(imgRelleno, vector<vector<Point>>{contorno}, -1, Scalar(255), FILLED);
    }
}

void procesarImagenesEnCarpeta(AAssetManager* assetManager, const string& rutaCarpeta, vector<pair<vector<double>, vector<double>>>& momentosHuZernikeFigura) {
    AAssetDir* assetDir = AAssetManager_openDir(assetManager, rutaCarpeta.c_str());
    if (assetDir == nullptr) {
        LOGE("No se puede abrir el directorio: %s", rutaCarpeta.c_str());
        return;
    }

    const char* archivo;
    while ((archivo = AAssetDir_getNextFileName(assetDir)) != nullptr) {
        if (string(archivo).find(".PNG") != string::npos) {
            string rutaArchivo = rutaCarpeta + "/" + archivo;
            LOGD("Intentando cargar: %s", rutaArchivo.c_str());

            AAsset* asset = AAssetManager_open(assetManager, rutaArchivo.c_str(), AASSET_MODE_BUFFER);
            if (asset == nullptr) {
                LOGE("Error: No se pudo cargar la imagen %s", rutaArchivo.c_str());
                continue;
            }

            off_t assetLength = AAsset_getLength(asset);
            vector<char> fileData(assetLength);
            AAsset_read(asset, fileData.data(), assetLength);
            AAsset_close(asset);

            Mat imagen = imdecode(Mat(fileData), IMREAD_COLOR);
            if (imagen.empty()) {
                LOGE("Error: No se pudo decodificar la imagen %s", rutaArchivo.c_str());
                continue;
            }

            Mat imagenHSV, mascara, imagenRelleno;
            cvtColor(imagen, imagenHSV, COLOR_BGR2HSV);
            inRange(imagenHSV, Scalar(100, 30, 40), Scalar(160, 255, 255), mascara);

            // Aplicar filtro de mediana
//            medianBlur(mascara, mascara, 3);

            // Mejorar la máscara usando operaciones morfológicas
            Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
            morphologyEx(mascara, mascara, MORPH_CLOSE, kernel);

            rellenar(mascara, imagenRelleno);

            double momentosHu[7];
            calcularMomentosHu(imagenRelleno, momentosHu);

            vector<double> momentosZernike;
            calcularMomentosZernike(imagenRelleno, momentosZernike, 4);

            momentosHuZernikeFigura.emplace_back(make_pair(vector<double>(momentosHu, momentosHu + 7), momentosZernike));

            LOGD("Momentos de Hu y Zernike calculados para %s", rutaArchivo.c_str());
        }
    }
    AAssetDir_close(assetDir);
}

string identificarFigura(const vector<double>& momentosHu, const vector<double>& momentosZernike) {
    if (momentosHuZernikeCirculos.empty() || momentosHuZernikeCuadrados.empty() || momentosHuZernikeTriangulos.empty()) {
        return "Desconocido";
    }

    double menorDistanciaHu = DBL_MAX;
    string figuraHu = "Desconocido";
    double menorDistanciaZernike = DBL_MAX;
    string figuraZernike = "Desconocido";

    for (const auto& momentos : momentosHuZernikeCirculos) {
        double distanciaHu = distanciaEuclidea(momentosHu, momentos.first);
        double distanciaZernike = distanciaEuclidea(momentosZernike, momentos.second);
        if (distanciaHu < menorDistanciaHu) {
            menorDistanciaHu = distanciaHu;
            figuraHu = "Circulo";
        }
        if (distanciaZernike < menorDistanciaZernike) {
            menorDistanciaZernike = distanciaZernike;
            figuraZernike = "Circulo";
        }
    }

    for (const auto& momentos : momentosHuZernikeCuadrados) {
        double distanciaHu = distanciaEuclidea(momentosHu, momentos.first);
        double distanciaZernike = distanciaEuclidea(momentosZernike, momentos.second);
        if (distanciaHu < menorDistanciaHu) {
            menorDistanciaHu = distanciaHu;
            figuraHu = "Cuadrado";
        }
        if (distanciaZernike < menorDistanciaZernike) {
            menorDistanciaZernike = distanciaZernike;
            figuraZernike = "Cuadrado";
        }
    }

    for (const auto& momentos : momentosHuZernikeTriangulos) {
        double distanciaHu = distanciaEuclidea(momentosHu, momentos.first);
        double distanciaZernike = distanciaEuclidea(momentosZernike, momentos.second);
        if (distanciaHu < menorDistanciaHu) {
            menorDistanciaHu = distanciaHu;
            figuraHu = "Triangulo";
        }
        if (distanciaZernike < menorDistanciaZernike) {
            menorDistanciaZernike = distanciaZernike;
            figuraZernike = "Triangulo";
        }
    }

    LOGD("Figura HU: %s", figuraHu.c_str());
    LOGD("Figura ZER: %s", figuraZernike.c_str());


    if (figuraHu == figuraZernike) {
        return figuraHu;
    } else {
        if (figuraHu < figuraZernike){
            return figuraHu;
        }else{
            return figuraZernike;
        }
        return "Desconocido";
    }
}

void preprocesamiento(const Mat& frame, Mat& framePreprocesado) {
    Mat imagenHSV, mascara, imagenRelleno;

    cvtColor(frame, imagenHSV, COLOR_BGR2HSV);

    inRange(imagenHSV, Scalar(60, 25, 30), Scalar(160, 255, 255), mascara);

//    // Aplicar filtro de mediana
    medianBlur(mascara, mascara, 3);

    // Mejorar la máscara usando operaciones morfológicas
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(mascara, mascara, MORPH_CLOSE, kernel);

    rellenar(mascara, imagenRelleno);

    framePreprocesado = imagenRelleno;
}

void detectar(Mat& frame, bool modoDibujar) {
    Mat framePreprocesado;
    preprocesamiento(frame, framePreprocesado);

    Moments momentos = moments(framePreprocesado, true);

    if (momentos.m00 < 500) {
        return;
    }

    double momentosHu[7];
    calcularMomentosHu(framePreprocesado, momentosHu);

    vector<double> momentosZernike;
    calcularMomentosZernike(framePreprocesado, momentosZernike, 4);

    string figuraIdentificada = identificarFigura(vector<double>(momentosHu, momentosHu + 7), momentosZernike);
//    LOGD("Figura identificada: %s", figuraIdentificada.c_str());

    frame = framePreprocesado;
    if (modoDibujar && momentos.m00 > 0) {
        int x = static_cast<int>(momentos.m10 / momentos.m00);
        int y = static_cast<int>(momentos.m01 / momentos.m00);
        putText(frame, figuraIdentificada, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
    }
}
