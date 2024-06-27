#include "parte2.h"
#include "LBP.cpp" // Incluir el archivo LBP.cpp
#include <android/log.h>
#include <android/asset_manager.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <cfloat>
#include <cmath>

#define LOG_TAG "PROYECTO_VISION"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;
using namespace std;

vector<pair<vector<int>, string>> clasificadoresLBPClase1;
vector<pair<vector<int>, string>> clasificadoresLBPClase2;

double distanciaEuclidea(const vector<int>& hist1, const vector<int>& hist2) {
    double suma = 0;
    for (size_t i = 0; i < hist1.size(); ++i) {
        suma += pow(hist1[i] - hist2[i], 2);
    }
    return sqrt(suma);
}

void procesarImagenesEnCarpetaLBP(AAssetManager* assetManager, const string& rutaCarpeta, vector<pair<vector<int>, string>>& clasificadoresLBP) {
    AAssetDir* assetDir = AAssetManager_openDir(assetManager, rutaCarpeta.c_str());
    if (assetDir == nullptr) {
        LOGE("No se puede abrir el directorio: %s", rutaCarpeta.c_str());
        return;
    }

    const char* archivo;
    while ((archivo = AAssetDir_getNextFileName(assetDir)) != nullptr) {
        if (string(archivo).find(".jpg") != string::npos) {
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

            Mat imagen = imdecode(Mat(fileData), IMREAD_GRAYSCALE);
            if (imagen.empty()) {
                LOGE("Error: No se pudo decodificar la imagen %s", rutaArchivo.c_str());
                continue;
            }

            vector<int> histoLBP = calcularLBP(imagen);
            clasificadoresLBP.emplace_back(histoLBP, rutaCarpeta);

            LOGD("Histograma LBP calculado y almacenado para %s", rutaArchivo.c_str());
        }
    }
    AAssetDir_close(assetDir);
}

string identificarFiguraLBP(const Mat& imagen) {
    vector<int> histoLBP = calcularLBP(imagen);

    double menorDistancia = DBL_MAX;
    string claseIdentificada = "Desconocido";

    for (const auto& clasificador : clasificadoresLBPClase1) {
        double distancia = distanciaEuclidea(histoLBP, clasificador.first);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            claseIdentificada = "Disco";
        }
    }

    for (const auto& clasificador : clasificadoresLBPClase2) {
        double distancia = distanciaEuclidea(histoLBP, clasificador.first);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            claseIdentificada = "Jirafa";
        }
    }

    LOGD("Clase identificada: %s", claseIdentificada.c_str());
    return claseIdentificada;
}

void detectarLBP(Mat& frame) {
    string figuraIdentificada = identificarFiguraLBP(frame);

    // Calcular el factor de escala basado en la altura de la imagen
    double scaleFactor = frame.rows / 500.0;

    // Ajustar el grosor del contorno del texto para que sea proporcional
    int thickness = static_cast<int>(2 * scaleFactor);

    // Poner el texto en la imagen usando el factor de escala
    putText(frame, figuraIdentificada, Point(10, frame.rows - 50), FONT_HERSHEY_SIMPLEX, scaleFactor, Scalar(255, 0, 0), thickness);
}
