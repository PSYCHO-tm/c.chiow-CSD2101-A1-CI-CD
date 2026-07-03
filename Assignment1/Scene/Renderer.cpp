/**
 * Renderer.cpp
 *
 * Implementation of the Renderer singleton.
 * This is the ONLY file that #includes Triangle.h and Square.h.
 * All other files (main.cpp, NativeTemplate.cpp) only see Renderer.h.
 */

#include "Renderer.h"
#include "Triangle.h"
#include "Square.h"

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

Renderer& Renderer::Instance()
{
    static Renderer instance;
    return instance;
}

// ---------------------------------------------------------------------------
// Android asset manager wiring
// ---------------------------------------------------------------------------

#ifdef PLATFORM_ANDROID
void Renderer::setAssetManager(AAssetManager* mgr)
{
    assetMgr = mgr;
}
#endif

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

bool Renderer::initializeRenderer()
{
    createModels();
    initializeModels();
    return true;
}

void Renderer::resize(int w, int h)
{
    for (Model* m : models)
        m->Resize(w, h);
}

void Renderer::render()
{
    for (Model* m : models)
        m->Render();
}

Renderer::~Renderer()
{
    clearModels();
}

// ---------------------------------------------------------------------------
// Model management
// ---------------------------------------------------------------------------

void Renderer::createModels()
{
    clearModels();
#ifdef PLATFORM_ANDROID
    models.push_back(new Triangle(assetMgr));
    models.push_back(new Square(assetMgr));
#else
    models.push_back(new Triangle());
    models.push_back(new Square());
#endif
}

void Renderer::initializeModels()
{
    for (Model* m : models)
        m->InitModel();
}

void Renderer::clearModels()
{
    for (Model* m : models)
        delete m;
    models.clear();
}
