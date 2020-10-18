#include "Demo.h"
#include "Eldur.h"

namespace ojgl {

ojstd::shared_ptr<Demo> getDemo(DemoType type)
{
    switch (type) {
    case DemoType::Eldur:
        return ojstd::make_shared<Eldur>();
    default:
        return nullptr;
    }
}

}