#include "OJGL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty\stb_image.h"
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

unsigned char song[] = {
#include "songs/song.inc"
};

std::string vertexShader{
#include "shaders/demo.vs"
};

std::string fragmentShader{
#include "shaders/demo.fs"
};

std::string vertexShaderPost{
#include "shaders/post.vs"
};

std::string fragmentShaderPost{
#include "shaders/post.fs"
};

#define SHADER_FRAGMENT_DOF_SCENE "shaders/dofScene.fs"
std::string fragmentDOFScene{
#include SHADER_FRAGMENT_DOF_SCENE
};

#define SHADER_FRAGMENT_DOF_BLUR1 "shaders/dofBlur1.fs"
std::string fragmentDOFBlur1{
#include SHADER_FRAGMENT_DOF_BLUR1
};

#define SHADER_FRAGMENT_DOF_BLUR2 "shaders/dofBlur2.fs"
std::string fragmentDOFBlur2{
#include SHADER_FRAGMENT_DOF_BLUR2
};

#define SHADER_FRAGMENT_DOF_FINAL "shaders/dofFinal.fs"
std::string fragmentDOFFinal{
#include SHADER_FRAGMENT_DOF_FINAL
};

#define SHADER_FRAGMENT_TUNNEL_SCENE "shaders/tunnelScene.fs"
std::string fragmentTunnelScene{
#include SHADER_FRAGMENT_TUNNEL_SCENE
};

#define SHADER_FRAGMENT_BASE_SCENE "shaders/demo.fs"
std::string fragmentBaseScene{
#include SHADER_FRAGMENT_BASE_SCENE
};

using namespace ojgl;

#ifdef _DEBUG
void debugRereadShaderFiles()
{
    std::unordered_map<std::string*, std::string> shaders;
    shaders[&fragmentShader] = "examples/shaders/demo.fs";
    shaders[&fragmentShaderPost] = "examples/shaders/post.fs";

    shaders[&fragmentDOFScene] = "examples/" SHADER_FRAGMENT_DOF_SCENE;
    shaders[&fragmentDOFBlur1] = "examples/" SHADER_FRAGMENT_DOF_BLUR1;
    shaders[&fragmentDOFBlur2] = "examples/" SHADER_FRAGMENT_DOF_BLUR2;
    shaders[&fragmentDOFFinal] = "examples/" SHADER_FRAGMENT_DOF_FINAL;

    shaders[&fragmentTunnelScene] = "examples/" SHADER_FRAGMENT_TUNNEL_SCENE;

    shaders[&fragmentBaseScene] = "examples/" SHADER_FRAGMENT_BASE_SCENE;

    for (auto[stringptr, path] : shaders) {
        std::ifstream shaderFile(path);
        _ASSERTE(!shaderFile.fail());

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        std::string fileContents = buffer.str();
        std::string pre = "R\"\"(";
        std::string post = ")\"\"";
        size_t start = fileContents.find(pre);
        size_t end = fileContents.rfind(post);
        std::string shader = fileContents.substr(start + pre.length(), end - start - pre.length());
        *stringptr = shader;
    }
}
#endif

void buildSceneGraph(GLState& glState)
{
    glState.clearScenes();

    auto pre = Buffer::construct(1024, 768, "main", vertexShader, fragmentShader);
    auto post = Buffer::construct(1024, 768, "post", vertexShaderPost, fragmentShaderPost);

    auto DOFScene = Buffer::construct(1024, 768, "DOFScene", vertexShader, fragmentDOFScene);
    auto DOFBlur1 = Buffer::construct(1024, 768, "DOFBlur1", vertexShader, fragmentDOFBlur1, { DOFScene });
    auto DOFBlur2 = Buffer::construct(1024, 768, "DOFBlur2", vertexShader, fragmentDOFBlur2, { DOFBlur1 });
    auto DOFFinal = Buffer::construct(1024, 768, "DOFFinal", vertexShader, fragmentDOFFinal, { DOFScene, DOFBlur2, DOFBlur1 });

    auto tunnel = Buffer::construct(1024, 768, "tunnel", vertexShader, fragmentTunnelScene);

    auto base = Buffer::construct(1024, 768, "base", vertexShader, fragmentBaseScene);

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
    const auto desiredFrameTime = Duration::milliseconds(17);

    Window window(1024, 768, false);
    GLState glState;

    Music music(song);
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
            if (key == Window::KEY_F1) {
                debugRereadShaderFiles();
                buildSceneGraph(glState);
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
