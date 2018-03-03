#include "Scene.h"
#include "utility\Log.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace ojgl {

Scene::Scene(std::shared_ptr<Buffer> buffer, timer::ms_t duration)
    : _mainBuffer(std::move(buffer))
    , _duration(duration)
{
    for (auto& b : buffers()) {
        if (b != _mainBuffer) {
            b->generateFBO();
        }
    }
}

Buffer& Scene::operator[](const std::string& name)
{
    auto buffers = this->buffers();
    auto res = std::find_if(buffers.begin(), buffers.end(), [&](auto b) { return b->name() == name; });
    _ASSERTE(res != buffers.end());
    return **res;
}

std::unordered_set<std::shared_ptr<Buffer>> Scene::buffers()
{
    std::unordered_set<std::shared_ptr<Buffer>> available;
    std::unordered_set<std::shared_ptr<Buffer>> checked;
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

timer::ms_t Scene::duration() const
{
    return this->_duration;
}

void Scene::render()
{

    auto available = buffers();
    std::unordered_set<std::shared_ptr<Buffer>> rendered;

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
            cur->render();
            rendered.insert(cur);
            curIter = available.erase(curIter);
        } else {
            curIter++;
        }
    }
}

} //namespace ojgl