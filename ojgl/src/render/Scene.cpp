#include "Scene.h"
#include "utility/OJstd.h"
#include "utility/Vector.h"

namespace ojgl {

Scene::Scene(const ojstd::string& name, const ojstd::shared_ptr<Buffer>& buffer, Duration duration)
    : _mainBuffer(buffer)
    , _duration(duration)
    , _name(name)

{
    for (auto& b : this->buffers()) {
        if (b != _mainBuffer) {
            b->generateFBO();
        }
    }
}

Scene::Scene(const ojstd::shared_ptr<Buffer>& buffer, Duration duration)
    : Scene("default", buffer, duration)
{
}

ojstd::string Scene::name() const
{
    return _name;
}

Buffer& Scene::operator[](const ojstd::string& name)
{
    auto buffers = this->buffers();
    auto res = ojstd::find_if(buffers.begin(), buffers.end(), [&](auto b) { return b->name() == name; });
    _ASSERTE(res != buffers.end());
    return **res;
}

ojstd::unordered_set<ojstd::shared_ptr<Buffer>> Scene::buffers()
{
    ojstd::unordered_set<ojstd::shared_ptr<Buffer>> available;
    ojstd::unordered_set<ojstd::shared_ptr<Buffer>> checked;
    available.insert(_mainBuffer);

    while (!available.empty()) {
        auto cur = *available.begin();

        // Check this node
        checked.insert(cur);

        for (auto& v : *cur) {
            //If a child node has not been checked, add to available
            if (checked.find(v) == checked.end()) {
                available.insert(v);
            }
        }
        available.erase(cur);
    }

    return checked;
}

Duration Scene::duration() const
{
    return this->_duration;
}

void Scene::render(const Vector2i& viewportOffset)
{

    auto available = buffers();
    ojstd::unordered_set<ojstd::shared_ptr<Buffer>> rendered;

    auto curIter = available.begin();

    //Loop until all are rendered
    //TODO detect cyclic dependence
    while (!available.empty()) {
        if (curIter == available.end()) {
            curIter = available.begin();
        }

        auto cur = *curIter;

        //If all child nodes are rendered, render this
        bool allRendered = true;
        for (auto& c : *cur) {
            if (rendered.find(c) == rendered.end()) {
                allRendered = false;
                break;
            }
        }
        if (allRendered) {
            cur->render(cur == _mainBuffer ? viewportOffset : Vector2i(0, 0));
            rendered.insert(cur);
            curIter = available.erase(curIter);
        } else {
            curIter++;
        }
    }
}

} //namespace ojgl
