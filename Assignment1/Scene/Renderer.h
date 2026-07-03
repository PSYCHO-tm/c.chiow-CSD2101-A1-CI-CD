/**
 * Renderer.h
 *
 * Singleton controller that owns all Model instances.
 * main.cpp and NativeTemplate.cpp only talk to Renderer — they never know
 * which concrete shapes exist.  Triangle and Square are only mentioned
 * inside Renderer.cpp (in createModels()).
 *
 * On Android, Java passes the AAssetManager* here before calling
 * initializeRenderer(), so that models can read .glsl files from the APK.
 */

#pragma once

#include "Model.h"
#include <vector>

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class Renderer {
public:
    static Renderer& Instance();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

#ifdef PLATFORM_ANDROID
    void setAssetManager(AAssetManager* mgr);
#endif

    bool initializeRenderer();
    void resize(int w, int h);
    void render();

private:
    Renderer()  = default;
    ~Renderer();

    void createModels();
    void initializeModels();
    void clearModels();

#ifdef PLATFORM_ANDROID
    AAssetManager* assetMgr = nullptr;
#endif

    std::vector<Model*> models; // Task 3: collection of all renderable shapes
};
