/**
 * NativeTemplate.cpp
 *
 * Thin JNI bridge between the Java/Android layer and the Renderer.
 * No Triangle or Square is mentioned here — all rendering is delegated
 * to Renderer::Instance().
 *
 * Native method signatures match MainActivity.java:
 *   package com.example.glpiframeworkintro
 *   class   MainActivity
 */

#define LOG_TAG "GLPIFrameworkIntroNative"

#include <jni.h>
#include <android/asset_manager_jni.h>
#include "Platform.h"
#include "Renderer.h"

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_glpiframeworkintro_MainActivity_nativeInit(
        JNIEnv* env, jobject /*this*/, jobject assetManager)
{
    LOGI("nativeInit called");
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    Renderer::Instance().setAssetManager(mgr);
    return Renderer::Instance().initializeRenderer() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_glpiframeworkintro_MainActivity_nativeResize(
        JNIEnv* /*env*/, jobject /*this*/, jint width, jint height)
{
    Renderer::Instance().resize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_glpiframeworkintro_MainActivity_nativeRender(
        JNIEnv* /*env*/, jobject /*this*/)
{
    Renderer::Instance().render();
}