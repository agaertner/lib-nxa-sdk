#pragma once

namespace NexusSDK {
namespace UI {

class UIScale {
public:
    static void Set(float scale) { s_current = scale; }
    static float Get() { return s_current; }
    static float Apply(float value) { return value * s_current; }

private:
    static inline float s_current = 1.0f;
};

} // namespace UI
} // namespace NexusSDK
