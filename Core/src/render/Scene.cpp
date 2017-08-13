#include "Scene.h"

namespace ojgl {

Scene::Scene(std::shared_ptr<Buffer> buffer)
    : _mainBuffer(buffer)
{
}

Scene::~Scene()
{
}

void Scene::render()
{
    _mainBuffer->reset();
    _mainBuffer->render();
}

} //namespace ojgl