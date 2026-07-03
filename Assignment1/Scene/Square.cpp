/**
 * Square.cpp
 *
 * Solid yellow square — concrete Model implementation.
 * Positioned at the right side of the screen (clip-space offset +0.55 in X)
 * so it renders alongside the Triangle without fully overlapping it.
 * Owns its own VAO, VBOs, and shader program independent of Triangle.
 */

#define LOG_TAG "Square"

#include "Square.h"
#include "ShaderHelper.h"

// ---------------------------------------------------------------------------
// Vertex data — two triangles forming a square, offset right in clip space
// ---------------------------------------------------------------------------

// Square centred at (0.75, 0.35), half-size 0.18 — fully right of the triangle
static const GLfloat kPositions[] = {
    // triangle 1
     0.57f,  0.53f,
     0.57f,  0.17f,
     0.93f,  0.53f,
    // triangle 2
     0.93f,  0.53f,
     0.57f,  0.17f,
     0.93f,  0.17f,
};

// All yellow
static const GLfloat kColors[] = {
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
};

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

#ifdef PLATFORM_ANDROID
Square::Square(AAssetManager* assetMgr) : mgr(assetMgr) {}
#else
Square::Square() = default;
#endif

Square::~Square()
{
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vboPos);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

// ---------------------------------------------------------------------------
// Model lifecycle
// ---------------------------------------------------------------------------

void Square::InitModel()
{
    LOGI("Square::InitModel");

#ifdef PLATFORM_ANDROID
    programID = ShaderHelper::buildProgramFromAssets(mgr,
        "shader/SquareVertex.glsl",
        "shader/SquareFragment.glsl");
#else
    programID = ShaderHelper::buildProgramFromFile(
        "SquareVertex.glsl",
        "SquareFragment.glsl");
#endif

    if (!programID) { LOGE("Square: could not create program"); return; }

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

    LOGI("Square::InitModel done");
}

void Square::Render()
{
    glUseProgram(programID);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Square::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
}
