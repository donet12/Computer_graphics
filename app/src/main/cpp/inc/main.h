//
// Created by I.B. Park on 2019-04-25.
//

#ifndef HW3_MAIN_H
#define HW3_MAIN_H

#include "global.h"
#include "scene.h"


void surfaceCreated(AAssetManager* aAssetManager);
void surfaceChanged(int width, int height);
void drawFrame(float deltaTime);
void setButtonFlag(bool upperFlag);
void setLowerFlag(bool lowerFlag);

extern "C"
JNIEXPORT void JNICALL
Java_korea_media_hw3_MainActivity_surfaceCreated(JNIEnv* env, jobject instance, jobject assetManager) {
    surfaceCreated(AAssetManager_fromJava(env, assetManager));
}

extern "C"
JNIEXPORT void JNICALL
Java_korea_media_hw3_MainActivity_surfaceChanged(JNIEnv* env, jobject instance, jint width, jint height) {
    surfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_korea_media_hw3_MainActivity_drawFrame(JNIEnv* env, jobject instance, jfloat deltaTime) {
    drawFrame(deltaTime);
}

extern "C"
JNIEXPORT void JNICALL
Java_korea_media_hw3_MainActivity_setButtonFlag(JNIEnv* env, jobject instance, jboolean flag){
    setButtonFlag(flag);
}


#endif //HW3_MAIN_H
