//
// Created by sebas on 26/6/24.
//

#ifndef PROYECTO_VISON_PARTE2_H
#define PROYECTO_VISON_PARTE2_H

#include <opencv2/opencv.hpp>
#include <android/asset_manager.h>
#include <vector>
#include <string>

using namespace cv;

extern std::vector<std::pair<std::vector<int>, std::string>> clasificadoresLBPClase1;
extern std::vector<std::pair<std::vector<int>, std::string>> clasificadoresLBPClase2;

void procesarImagenesEnCarpetaLBP(AAssetManager* assetManager, const std::string& rutaCarpeta, std::vector<std::pair<std::vector<int>, std::string>>& clasificadoresLBP);
std::string identificarFigura(const Mat& imagen);
void detectarLBP(Mat& frame);

#endif //PROYECTO_VISON_PARTE2_H
