#include "Scene.h"
//#include "utility\Log.h"
//#include <algorithm>
//#include <stdexcept>
#include "..\..\fakelibs\fakelib.h"

namespace ojgl {

Scene::Scene(const fl::string& name, const fl::shared_ptr<Buffer>& buffer /*, Duration duration*/)
    : _mainBuffer(std::move(buffer))
    /*, _duration(duration)*/
    , _name(name)

{
    for (auto& b : buffers()) {
        if (b != _mainBuffer) {
            b->generateFBO(); // error C2662: 'void ojgl::Buffer::generateFBO(void)': cannot convert 'this' pointer from 'const T' to 'ojgl::Buffer &'
        }
    }
}

Scene::Scene(const fl::shared_ptr<Buffer>& buffer /*, Duration duration*/)
    : Scene("default", buffer /*, duration*/)
{
}

fl::string Scene::name() const
{
    return _name;
}

Buffer& Scene::operator[](const fl::string& name)
{
    auto buffers = this->buffers();
    auto res = fl::find_if(buffers.begin(), buffers.end(), [&](auto b) { return b->name() == name; });
    _ASSERTE(res != buffers.end());
    return **res;
}

fl::vector<fl::shared_ptr<Buffer>> Scene::buffers()
{
    fl::vector<fl::shared_ptr<Buffer>> available;
    fl::vector<fl::shared_ptr<Buffer>> checked;
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

//Duration Scene::duration() const
//{
//    return this->_duration;
//}

void Scene::render()
{

    auto available = buffers();
    fl::vector<fl::shared_ptr<Buffer>> rendered;

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
