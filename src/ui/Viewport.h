#pragma once

#include "controls/Container.h"
#include <memory>
#include <string>

namespace NexusSDK {
namespace UI {

class Viewport : public Container {
public:
    Viewport();
    ~Viewport() = default;

    // Singleton access
    static void Initialize();
    static void Shutdown();
    static std::shared_ptr<Viewport> Get();

    // The main render hook to be called every frame
    static void Render(float uiScale = 1.0f);

    float GetCurrentScale() const { return m_currentScale; }

protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override {
        DrawChildren(bounds, scale);
    }

private:
    static std::shared_ptr<Viewport> s_instance;
    float m_currentScale = 1.0f;
};

} // namespace UI
} // namespace NexusSDK
