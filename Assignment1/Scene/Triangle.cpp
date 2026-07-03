/**
 * Triangle.cpp
 *
 * Rotating RGB triangle — concrete Model implementation.
 * Shaders are loaded from BlueTriangleVertex.glsl / BlueTriangleFragment.glsl
 * via ShaderHelper; no GLSL source is embedded as C-string literals here.
 */

#define LOG_TAG "Triangle"

#include "Triangle.h"
#include "ShaderHelper.h"
#include <cmath>

// ---------------------------------------------------------------------------
// Vertex data
// ---------------------------------------------------------------------------

static const GLfloat kPositions[] = {
     0.0f,  0.5f,
    -0.5f, -0.5f,
     0.5f, -0.5f
};

static const GLfloat kColors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

#ifdef PLATFORM_ANDROID
Triangle::Triangle(AAssetManager* assetMgr) : mgr(assetMgr) {}
#else
Triangle::Triangle() = default;
#endif

Triangle::~Triangle()
{
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vboPos);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

// ---------------------------------------------------------------------------
// Model lifecycle
// ---------------------------------------------------------------------------

void Triangle::InitModel()
{
    LOGI("Triangle::InitModel");

#ifdef PLATFORM_ANDROID
    programID = ShaderHelper::buildProgramFromAssets(mgr,
        "shader/BlueTriangleVertex.glsl",
        "shader/BlueTriangleFragment.glsl");
#else
    programID = ShaderHelper::buildProgramFromFile(
        "BlueTriangleVertex.glsl",
        "BlueTriangleFragment.glsl");
#endif

    if (!programID) { LOGE("Triangle: could not create program"); return; }

    uRadianAngle = glGetUniformLocation(programID, "RadianAngle");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions), kPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kColors), kColors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    LOGI("Triangle::InitModel done");
}

void Triangle::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programID);

    float radian = degree++ / 57.2957795f;
    glUniform1f(uRadianAngle, radian);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Triangle::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
}
