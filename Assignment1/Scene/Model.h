/**
 * Model.h
 *
 * Abstract base class defining the lifecycle every renderable shape must follow.
 * Renderer owns a collection of Model* and calls these methods each frame.
 * Concrete subclasses (Triangle, Square) implement InitModel() and Render();
 * Resize() is optional (default no-op) for shapes that don't care about viewport.
 */

#pragma once

class Model {
public:
    Model() {}
    virtual ~Model() {}

    virtual void InitModel() = 0;   // called once, before the first frame
    virtual void Render()   = 0;   // called once per frame
    virtual void Resize(int w, int h) {}  // called whenever the surface/window resizes
};
