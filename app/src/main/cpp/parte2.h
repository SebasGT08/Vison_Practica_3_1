//
// Created by sebas on 26/6/24.
//

#ifndef PROYECTO_VISON_PARTE2_H
#define PROYECTO_VISON_PARTE2_H

#include <opencv2/opencv.hpp>
#include <android/asset_manager.h>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

extern vector<pair<vector<int>, string>> clasificadoresLBPClase1;
extern vector<pair<vector<int>, string>> clasificadoresLBPClase2;

void procesarImagenesEnCarpetaLBP(AAssetManager* assetManager, const string& rutaCarpeta, vector<pair<vector<int>, string>>& clasificadoresLBP);
string identificarFiguraLBP(const Mat& imagen);
void detectarLBP(Mat& frame,Mat& histImage);

#endif //PROYECTO_VISON_PARTE2_H
