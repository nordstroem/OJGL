#include "Scene.h"
#include <set>
#include <stdexcept>

namespace ojgl {

Scene::Scene(std::shared_ptr<Buffer> buffer)
    : _mainBuffer(buffer)
{
    for (auto& b : buffers()) {
        if (b != _mainBuffer)
            b->generateFBO();
    }
}

Scene::~Scene()
{
}

Buffer& Scene::operator[](const std::string& name)
{
    for (auto& b : buffers()) {
        if (b->name() == name) {
            return *b;
        }
    }

    //TODO buffer not found, maybe throw runtime error
    return *_mainBuffer;
}

std::set<std::shared_ptr<Buffer>> Scene::buffers()
{
    std::set<std::shared_ptr<Buffer>> available;
    std::set<std::shared_ptr<Buffer>> checked;
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

void Scene::render()
{

    auto available = buffers();
    std::set<std::shared_ptr<Buffer>> rendered;

    auto curIter = available.begin();

    //Loop until all are rendered
    //TODO detect cyclic dependence
    while (!available.empty()) {
        if (curIter == available.end())
            curIter = available.begin();

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