//
// Created by sebas on 25/6/24.
//

#ifndef PROYECTO_VISON_PARTE1_H
#define PROYECTO_VISON_PARTE1_H

#include <opencv2/opencv.hpp>
#include <android/asset_manager.h>
#include <vector>
#include <string>

extern std::vector<std::pair<std::vector<double>, std::vector<double>>> momentosHuZernikeCirculos;
extern std::vector<std::pair<std::vector<double>, std::vector<double>>> momentosHuZernikeCuadrados;
extern std::vector<std::pair<std::vector<double>, std::vector<double>>> momentosHuZernikeTriangulos;

void calcularMomentosHu(const cv::Mat& imagen, double momentosHu[7]);
void logMomentosHuZernike(const std::string& tipoFigura, const std::vector<std::pair<std::vector<double>, std::vector<double>>>& momentosHuZernikeFigura);
double distanciaEuclidea(const std::vector<double>& momentos1, const std::vector<double>& momentos2);
double calcularDistanciaPonderada(const std::vector<double>& momentos1, const std::vector<double>& momentos2, double peso);
std::string identificarFigura(const std::vector<double>& momentosHu, const std::vector<double>& momentosZernike);
void esquelitizar(const cv::Mat& imgBinaria, cv::Mat& imgEsqueleto);
void procesarImagenesEnCarpeta(AAssetManager* assetManager, const std::string& rutaCarpeta, std::vector<std::pair<std::vector<double>, std::vector<double>>>& momentosHuZernikeFigura);
void preprocesamiento(const cv::Mat& frame, cv::Mat& framePreprocesado);
void detectar(cv::Mat& frame, bool modoDibujar);

#endif //PROYECTO_VISON_PARTE1_H
