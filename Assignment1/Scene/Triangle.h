/**
 * Triangle.h
 *
 * Concrete Model subclass: rotating RGB triangle.
 * Loads its shaders from BlueTriangleVertex.glsl / BlueTriangleFragment.glsl
 * via ShaderHelper (file-based path).
 */

#pragma once

#include "Platform.h"
#include "Model.h"

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class Triangle : public Model {
public:
#ifdef PLATFORM_ANDROID
    explicit Triangle(AAssetManager* assetMgr);
#else
    Triangle();
#endif
    ~Triangle() override;

    void InitModel()          override;
    void Render()             override;
    void Resize(int w, int h) override;

private:
#ifdef PLATFORM_ANDROID
    AAssetManager* mgr = nullptr;
#endif

    GLuint programID    = 0;
    GLuint vao          = 0;
    GLuint vboPos       = 0;
    GLuint vboColor     = 0;
    GLint  uRadianAngle = -1;
    float  degree       = 0.0f;
};
