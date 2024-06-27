#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "parte1.h" // Incluir el archivo parte1.h
#include "parte2.h" // Incluir el archivo parte1.h

#define LOG_TAG "PROYECTO_VISION"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

using namespace cv;

extern "C" JNIEXPORT void JNICALL
Java_com_example_proyecto_1vison_MainActivity_procesarFrame(
        JNIEnv* env, jobject, jlong direccionMatRgba, jboolean modoDibujar) {
    Mat& frame = *(Mat*)direccionMatRgba;
    cvtColor(frame, frame, COLOR_RGBA2BGR); // Convertir a BGR para el procesamiento

    if(modoDibujar){
        detectar(frame);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_proyecto_1vison_MainActivity_calcularMomentosHuYzernike(JNIEnv* env, jobject obj, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    std::string rutaBaseCarpeta = "images";  // Ruta base a tu carpeta de imágenes dentro de assets
    std::vector<std::string> subcarpetas = {"circle", "square", "triangle"};

    momentosHuZernikeCirculos.clear();
    momentosHuZernikeCuadrados.clear();
    momentosHuZernikeTriangulos.clear();

    for (const auto& subcarpeta : subcarpetas) {
        std::string rutaCarpeta = rutaBaseCarpeta + "/" + subcarpeta;
        LOGD("Procesando carpeta: %s", rutaCarpeta.c_str());
        if (subcarpeta == "circle") {
            procesarImagenesEnCarpeta(mgr, rutaCarpeta, momentosHuZernikeCirculos);
        } else if (subcarpeta == "square") {
            procesarImagenesEnCarpeta(mgr, rutaCarpeta, momentosHuZernikeCuadrados);
        } else if (subcarpeta == "triangle") {
            procesarImagenesEnCarpeta(mgr, rutaCarpeta, momentosHuZernikeTriangulos);
        }
    }

    LOGD("Momentos de Hu y Zernike calculados para todas las carpetas");
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_proyecto_1vison_ImagePickerActivity_procesarParte2(JNIEnv* env, jobject, jlong direccionMatRgba, jlong direccionMatRes) {
    Mat& matOriginal = *(Mat*)direccionMatRgba;
    Mat& matRes = *(Mat*)direccionMatRes;

    cvtColor(matOriginal, matRes, COLOR_RGBA2BGR);
    detectarLBP(matRes);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_proyecto_1vison_ImagePickerActivity_procesarImagenesLBP(JNIEnv* env, jobject obj, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    std::string rutaBaseCarpeta = "images";  // Ruta base a tu carpeta de imágenes dentro de assets
    std::vector<std::string> subcarpetas = {"disco", "jirafa"};

    clasificadoresLBPClase1.clear();
    clasificadoresLBPClase2.clear();

    for (const auto& subcarpeta : subcarpetas) {
        std::string rutaCarpeta = rutaBaseCarpeta + "/" + subcarpeta;
        LOGD("Procesando carpeta: %s", rutaCarpeta.c_str());
        if (subcarpeta == "disco") {
            procesarImagenesEnCarpetaLBP(mgr, rutaCarpeta, clasificadoresLBPClase1);
        } else if (subcarpeta == "jirafa") {
            procesarImagenesEnCarpetaLBP(mgr, rutaCarpeta, clasificadoresLBPClase2);
        }
    }

    LOGD("Histograma LBP calculado para todas las carpetas");
}
