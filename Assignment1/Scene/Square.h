/**
 * Square.h
 *
 * Concrete Model subclass: solid yellow square, offset to the right so it
 * renders alongside the Triangle without fully overlapping it.
 * Owns its own VAO, VBOs, and shader program — independent of Triangle.
 */

#pragma once

#include "Platform.h"
#include "Model.h"

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class Square : public Model {
public:
#ifdef PLATFORM_ANDROID
    explicit Square(AAssetManager* assetMgr);
#else
    Square();
#endif
    ~Square() override;

    void InitModel()          override;
    void Render()             override;
    void Resize(int w, int h) override;

private:
#ifdef PLATFORM_ANDROID
    AAssetManager* mgr = nullptr;
#endif

    GLuint programID = 0;
    GLuint vao       = 0;
    GLuint vboPos    = 0;
    GLuint vboColor  = 0;
};
