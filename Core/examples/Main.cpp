#include "OJGL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "EmbeddedResources.h"
#include "thirdparty\stb_image.h"
#include "utility\ShaderReader.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>
#include <unordered_map>

using namespace ojgl;

void buildSceneGraph(GLState& glState)
{
    glState.clearScenes();

    auto pre = Buffer::construct(1024, 768, "main", "demo.vs", "demo.fs");
    auto post = Buffer::construct(1024, 768, "post", "post.vs", "post.fs");

    auto DOFScene = Buffer::construct(1024, 768, "DOFScene", "demo.vs", "dofScene.fs");
    auto DOFBlur1 = Buffer::construct(1024, 768, "DOFBlur1", "demo.vs", "dofBlur1.fs", { DOFScene });
    auto DOFBlur2 = Buffer::construct(1024, 768, "DOFBlur2", "demo.vs", "dofBlur2.fs", { DOFBlur1 });
    auto DOFFinal = Buffer::construct(1024, 768, "DOFFinal", "demo.vs", "dofFinal.fs", { DOFScene, DOFBlur2, DOFBlur1 });

    auto tunnel = Buffer::construct(1024, 768, "tunnel", "demo.vs", "tunnelScene.fs");

    auto base = Buffer::construct(1024, 768, "base", "demo.vs", "demo.fs");

    glState.addScene(Scene{ "baseScene", base, Duration::milliseconds(3000000) });
    glState.addScene(Scene{ "DOFScene", DOFFinal, Duration::milliseconds(30000) });
    glState.addScene(Scene{ "tunnelScene", tunnel, Duration::milliseconds(30000) });
    glState.addScene(Scene{ "imageScene", pre, Duration::milliseconds(30000) });
}

std::tuple<int, int, int, std::unique_ptr<unsigned char, decltype(&stbi_image_free)>> readTexture(const std::string& filepath)
{
    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> dataptr(data, stbi_image_free);
    return std::make_tuple(width, height, channels, std::move(dataptr));
}

int main()
{

    ShaderReader::setBasePath("examples/shaders/");
    ShaderReader::preLoad("demo.vs", resources::vertex::demo);
    ShaderReader::preLoad("post.vs", resources::vertex::post);
    ShaderReader::preLoad("post.fs", resources::fragment::post);
    ShaderReader::preLoad("demo.fs", resources::fragment::demo);
    ShaderReader::preLoad("dofScene.fs", resources::fragment::dofScene);
    ShaderReader::preLoad("dofBlur1.fs", resources::fragment::dofBlur1);
    ShaderReader::preLoad("dofBlur2.fs", resources::fragment::dofBlur2);
    ShaderReader::preLoad("dofFinal.fs", resources::fragment::dofFinal);
    ShaderReader::preLoad("tunnel.fs", resources::fragment::tunnel);

    const auto desiredFrameTime = Duration::milliseconds(17);

    Window window(1024, 768, false);
    GLState glState;

    Music music(resources::songs::song);
    music.play();

    buildSceneGraph(glState);

    auto[width, height, channels, data] = readTexture("examples/textures/image.png");
    auto texture = Texture::construct(width, height, channels, data.get());

    glState["imageScene"]["main"] << Uniform1t("image", texture);
    glState.setStartTime(Timepoint::now());

    auto previousPrintTime = Timepoint::now();
    while (true) {
        Timer timer;
        timer.start();

        window.getMessages();

        for (auto key : window.getPressedKeys()) {
            if (key == Window::KEY_ESCAPE) {
                return 0;
            }
#ifdef _DEBUG
            bool timeChanged(false);
            LOG_INFO("key: " << key);
            if (key == Window::KEY_LEFT) {
                glState.changeTime(Duration::milliseconds(-1000));
                timeChanged = true;
            }
            if (key == Window::KEY_RIGHT) {
                glState.changeTime(Duration::milliseconds(1000));
                timeChanged = true;
            }
            if (key == Window::KEY_SPACE) {
                glState.togglePause();
                if (glState.isPaused()) {
                    music.stop();
                }
                timeChanged = true;
            }

            if (key == Window::KEY_R) {
                glState.restart();
                timeChanged = true;
            }
            if (key == Window::KEY_UP) {
                glState.nextScene();
                timeChanged = true;
            }
            if (key == Window::KEY_DOWN) {
                glState.previousScene();
                timeChanged = true;
            }
            if (!glState.isPaused() && timeChanged) {
                music.setTime(glState.elapsedTime());
            }
#endif
        }

        auto iGlobalTime = glState.relativeSceneTime();

        glState["baseScene"]["base"] << Uniform1f("iGlobalTime", iGlobalTime.toMilliseconds() / 1000.f);
        glState["tunnelScene"]["tunnel"] << Uniform1f("iGlobalTime", iGlobalTime.toMilliseconds() / 1000.f)
                                         << Uniform1f("CHANNEL_12_TOTAL", static_cast<GLfloat>(music.syncChannels()[12].getTotalHitsPerNote(0)))
                                         << Uniform1f("CHANNEL_13_TOTAL", static_cast<GLfloat>(music.syncChannels()[13].getTotalHitsPerNote(0)));

        for (auto& kv : music.syncChannels()) {
            const auto& sc = kv.second;
            std::vector<GLfloat> valuesSince;
            std::vector<GLfloat> valuesTo;

            for (int i = 0; i < sc.numNotes; i++) {
                valuesSince.push_back(static_cast<GLfloat>(sc.getTimeSinceLast(i).toMilliseconds()));
                valuesTo.push_back(static_cast<GLfloat>(sc.getTimeToNext(i).toMilliseconds()));
            }

            glState["baseScene"]["base"] << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_SINCE", valuesSince)
                                         << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_TO", valuesTo);
        }

        glState.render();
        if (!glState.isPaused()) {
            music.updateSync();
        }
        timer.end();

        auto timeSinceLastPrint = Timepoint::now() - previousPrintTime;
        if (timeSinceLastPrint > Duration::seconds(2)) {
            LOG_INFO("Frame time: " << timer.currentTime());
            previousPrintTime = Timepoint::now();
        }
    }
    return 0;
}
