#include "ControlBase.h"
#include "Container.h"

namespace NexusSDK {
namespace UI {

void ControlBase::SetParent(Container* parent) {
    if (m_parent == parent) return;

    if (m_parent) {
        Container* oldParent = m_parent;
        m_parent = nullptr;
        oldParent->RemoveChild(shared_from_this());
    }

    m_parent = parent;

    if (m_parent) {
        m_parent->AddChild(shared_from_this());
    }
}

} // namespace UI
} // namespace NexusSDK
