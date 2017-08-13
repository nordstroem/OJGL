#include "Scene.h"
#include <set>
#include <stdexcept>

namespace ojgl {

Scene::Scene(std::shared_ptr<Buffer> buffer)
    : _mainBuffer(buffer)
{
}

Scene::~Scene()
{
}

Buffer& Scene::operator[](const std::string& name)
{

    std::set<std::shared_ptr<Buffer>> available;
    std::set<std::shared_ptr<Buffer>> checked;
    available.insert(_mainBuffer);

    while (!available.empty()) {
        auto cur = *available.begin();

        // Check this node
        checked.insert(cur);
        if (cur->name() == name) {
            return *cur;
        } else {

            for (auto& v : *cur) {
                //If a child node has not been checked, add to available
                if (checked.find(v) != checked.end()) {
                    available.insert(v);
                }
            }
            available.erase(cur);
        }
    }

    throw std::invalid_argument("Can not find a buffer with that name");
    return *_mainBuffer;
}

void Scene::render()
{
    _mainBuffer->reset();
    _mainBuffer->render();
}

} //namespace ojgl