
#include "EmbeddedResources.h"
#include "FreeCameraController.h"
#include "render/GLState.h"
#include "render/Popup.h"
#include "render/Texture.h"
#include "render/Window.h"
#include "utility/Log.h"
#include "utility/Macros.h"
#include "utility/OJstd.h"
#include "utility/ShaderReader.h"

using namespace ojgl;

Vector2i calculateDimensions(float demoAspectRatio, int windowWidth, int windowHeight);
void buildSceneGraph(GLState& glState, int width, int height);

static constexpr int NUM_SCENES = 5;
static float sceneLengths[NUM_SCENES] = { 20., 10., 5., 5., 20 };

int currentSubScene(float iTime)
{
    float s = iTime;
    for (int i = 0; i < NUM_SCENES; i++) {
        s -= sceneLengths[i];
        if (s < 0)
            return i;
    }
    return NUM_SCENES;
}

float localSubTime(float iTime)
{
    float s = iTime;
    for (int i = 0; i < NUM_SCENES; i++) {
        if (s - sceneLengths[i] < 0)
            return s;
        s -= sceneLengths[i];
    }
    return s;
}

float localSubTimeLeft(float iTime)
{
    float s = iTime;
    for (int i = 0; i < NUM_SCENES; i++) {
        if (s - sceneLengths[i] < 0)
            return sceneLengths[i] - s;
        s -= sceneLengths[i];
    }
    return 99999999999.;
}

Vector3f spherePosition;
float previousTime = 0.f;

static auto generateRandomMatrices(int number)
{
    ojstd::vector<Matrix> matrices;
    for (int i = 0; i < number; i++)
        matrices.push_back(Matrix::randomRotation());
    return matrices;
}

ojstd::vector<Matrix> randomMatrices = generateRandomMatrices(500);
int prevCs = -1;
void handleSphereScene(GLState& state, FreeCameraController& cameraController, const ojstd::shared_ptr<Mesh>& sphere)
{
    float baseTime = state.relativeSceneTime().toSeconds();
    int cs = currentSubScene(baseTime);
    float lTime = localSubTime(baseTime);
    float lTimeLeft = localSubTimeLeft(baseTime);

    /*float k = 2.f;
    auto [fraction, base] = ojstd::modf(baseTime / k);
    float time = k * fraction;
    float gf = 0.2;

    int num = 600;
    for (int i = 0; i < num; i++) {
        float v0 = (22.f + 5 * ojstd::sin(1.f * (i + 5))) * 0.3;
        float beta = 0 + (i - num / 2) / 580.f;
        float alpha = 0.26f + 0.2 * ojstd::sin(1.f * i + time);

        spherePosition.x = 0.5f + v0 * ojstd::cos(alpha) * ojstd::cos(beta) * time;
        spherePosition.z = v0 * ojstd::cos(alpha) * ojstd::sin(beta) * time;
        spherePosition.y = 0.3f + v0 * ojstd::sin(alpha) * time - time * time * gf;
        state["meshScene"]["sphere"].insertMesh(sphere, Matrix::translation(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix::scaling(0.02f));
    }*/

    if (cs == 0 && prevCs != cs)
        // cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
        cameraController.set({ 2.57, -0.375, 7.71 }, 0.36, 0.08);

    if (cs == 2) {
        float f = ojstd::smoothstep(0, 2, lTime);
        float xPos = ojstd::lerp(3.3, -1.27, f);
        float heading = ojstd::lerp(0.36, -0.18, f);
        cameraController.set({ xPos, 5.67, 8.5 }, heading, -0.6);
        //      cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
        //     cameraController.set({ -1.27, 5.67, 8.66 }, -0.18, -0.6);
    }
    //    cameraController.set({ -1.27, 5.67, 8.66 }, -0.18, -0.6);

    if (cs == 0 || cs == 1) {
        float r = 4;
        float w = 1.;
        float sphereX = r * ojstd::cos(baseTime * w);
        float sphereZ = r * ojstd::sin(baseTime * w);
        float sc = ojstd::smoothstep(10, 12, baseTime) * 0.02;
        state["meshScene"]["sphere"].insertMesh(sphere, Matrix::translation(sphereX, 0, sphereZ) * Matrix::scaling(sc));

        if (lTimeLeft < 3.)
            cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
    }

    if (cs == 1 || cs == 2) {
        for (auto& m : randomMatrices) {
            float r = 7;
            float w = 0.1;
            float sphereX = r * ojstd::cos(baseTime * w);
            float sphereZ = r * ojstd::sin(baseTime * w);
            float sc = cs == 2 ? 0.01 : ojstd::smoothstep(0, 5, lTime) * 0.01;
            state["meshScene"]["sphere"].insertMesh(sphere, m * Matrix::translation(sphereX, 0, sphereZ) * Matrix::scaling(sc));
        }
    }

    if (cs == 3 && prevCs != cs) {
        cameraController.set({ -2.71, 0.79, 4.99 }, -0.64, -0.22);
    }

    prevCs = cs;
}

int main(int argc, char* argv[])
{
    //auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = 1280;
    int height = 720;
    bool fullScreen = false;
    bool showCursor = !fullScreen;

    ShaderReader::setBasePath("examples/");
    ShaderReader::preLoad("shaders/common/noise.fs", resources::fragment::noise);
    ShaderReader::preLoad("shaders/common/primitives.fs", resources::fragment::primitives);
    ShaderReader::preLoad("shaders/common/raymarch_utils.fs", resources::fragment::raymarch_utils);
    ShaderReader::preLoad("shaders/common/utils.fs", resources::fragment::utils);
    ShaderReader::preLoad("shaders/geometry-with-physics/mesh.fs", resources::fragment::mesh);
    ShaderReader::preLoad("shaders/geometry-with-physics/mesh.vs", resources::vertex::mesh);
    ShaderReader::preLoad("shaders/geometry-with-physics/rayMarch.fs", resources::fragment::rayMarch);
    ShaderReader::preLoad("shaders/geometry-with-physics/rayMarch.vs", resources::vertex::rayMarch);
    ShaderReader::preLoad("shaders/geometry-with-physics/smoke.fs", resources::fragment::smoke);
    ShaderReader::preLoad("shaders/geometry-with-physics/smoke.vs", resources::vertex::smoke);
    ShaderReader::preLoad("shaders/geometry-with-physics/sphere.fs", resources::fragment::sphere);
    ShaderReader::preLoad("shaders/geometry-with-physics/sphere.vs", resources::vertex::sphere);

    // @todo move this into GLState? We can return a const reference to window.
    // and perhaps have a unified update() which does getMessages(), music sync update and
    // so on.
    Window window(width, height, "Eldur - OJ", fullScreen, showCursor);
    GLState glState(resources::songs::song);

    auto [sceneWidth, sceneHeight] = calculateDimensions(16.0f / 9.0f, width, height);
    Vector2i viewportOffset((width - sceneWidth) / 2, (height - sceneHeight) / 2);

    buildSceneGraph(glState, sceneWidth, sceneHeight);
    glState.initialize();
    FreeCameraController cameraController;

    auto mesh = Mesh::constructIndexedQuad();
    auto sphere = Mesh::constructCube();
    auto randomRotation = Matrix::randomRotation();

    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
        cameraController.update(window);
        window.getMessages();

        for (auto key : window.getPressedKeys()) {
            switch (key) {
            case Window::KEY_ESCAPE:
                return 0;
#ifdef _DEBUG
            case Window::KEY_LEFT:
                glState.changeTime(Duration::milliseconds(-5000));
                break;

            case Window::KEY_RIGHT:
                glState.changeTime(Duration::milliseconds(5000));
                break;

            case Window::KEY_SPACE:
                glState.togglePause();
                break;

            case Window::KEY_R:
                glState.restart();
                break;

            case Window::KEY_UP:
                glState.nextScene();
                break;

            case Window::KEY_DOWN:
                glState.previousScene();
                break;

            case Window::KEY_C:
                LOG_INFO("Camera: (" << cameraController.position.x << ", " << cameraController.position.y << ", " << cameraController.position.z << ")"
                                     << ", [" << cameraController.heading << ", " << cameraController.elevation << "]");
                break;
#endif
            }
        }

        float baseTime = glState.relativeSceneTime().toSeconds();
        auto [fraction, base] = ojstd::modf(baseTime);
        float time = base + ojstd::pow(fraction, 2);

        handleSphereScene(glState, cameraController, sphere);

        Matrix cameraMatrix = cameraController.getCameraMatrix();
        Matrix cameraMatrixInverse = cameraMatrix.inverse();

        auto billboardMatrix = [&cameraMatrixInverse](const Vector3f& position, const float scaling) {
            const float* md = cameraMatrixInverse.data();
            float bd[16] = { md[0] * scaling, md[4], md[8], 0, md[1], md[5] * scaling, md[9], 0, md[2], md[6], md[10] * scaling, 0, position.x, position.y, position.z, 1 };
            return Matrix(bd);
        };

        //for (int i = 0; i < 10; i++) {
        //    glState["meshScene"]["mesh"].insertMesh(mesh, billboardMatrix({ 1.0f + i / 2.f, i / 5.f, 0.f }, 1.0));
        //}

        //glState["meshScene"]["mesh"].insertMesh(mesh, billboardMatrix({ 3.0f, 1.f, 0.f }, 1.0));

        /*for (int i = 0; i < 200; i++) {
            float xPos = (i / 200.f - 0.5f) * 4;
            float yPos = ojstd::sin(xPos * 5.f + 0.1 * time) * ojstd::cos(xPos * 2.5f + 1.0 * time);
            float zPos = ojstd::sin(xPos * 4.f + 0.1 * time) * ojstd::cos(yPos * 3.5f + 1.0 * time);
            glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(xPos, yPos, zPos) * Matrix::rotation(1, 1, 1, time + i) * Matrix::scaling(0.06f * ojstd::sin(xPos + time)));
        }*/
        /*
        for (int i = 0; i < 200; i++) {
            baseTime += 5.;
            float xPos = (i / 200.f - 0.5f) * 4;
            float yPos = 2 * ojstd::sin(xPos * 5.f + 0.1 * baseTime) * ojstd::cos(xPos * 2.5f + 1.0 * baseTime);
            float zPos = 0.1 * ojstd::sin(xPos * 4.f + 0.1 * baseTime) * ojstd::cos(yPos * 3.5f + 1.0 * baseTime);
            glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(xPos, yPos, zPos) * Matrix::rotation(1, 1, 1, baseTime + i) * Matrix::scaling(0.01f));
        }
		*/
        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(1, 0, 0) * Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));

        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.4f) * Matrix::translation(0.3, ojstd::sin(glState.relativeSceneTime().toSeconds()), 0.0));

        float fov = 0.927295218f;
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        glState << UniformMatrix4fv("P", Matrix::perspective(fov, aspectRatio, 0.001f, 100.0f) * cameraMatrixInverse);
        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
        glState << Uniform2f("iResolution", static_cast<float>(sceneWidth), static_cast<float>(sceneHeight));
        glState << UniformMatrix4fv("iCameraMatrix", cameraMatrix);
        glState.update(viewportOffset);

        timer.end();
#ifdef _DEBUG
        ojstd::string debugTitle("Frame time: ");
        debugTitle.append(ojstd::to_string(timer.elapsed().toMilliseconds<long>()));
        // debugTitle.append(" ms, fps: ");
        // debugTitle.append(ojstd::to_string(static_cast<int>(1000 / timer.elapsed().toMilliseconds())));
        debugTitle.append(" scene time: ");
        debugTitle.append(ojstd::to_string(static_cast<int>(glState.relativeSceneTime().toSeconds())));

        window.setTitle(debugTitle);
#endif
    }
}

Vector2i calculateDimensions(float demoAspectRatio, int windowWidth, int windowHeight)
{
    float windowAspectRatio = static_cast<float>(windowWidth) / windowHeight;

    if (demoAspectRatio > windowAspectRatio) {
        return Vector2i(windowWidth, ojstd::ftoi(windowWidth / demoAspectRatio));
    } else {
        return Vector2i(ojstd::ftoi(windowHeight * demoAspectRatio), windowHeight);
    }
}

void buildSceneGraph(GLState& glState, int width, int height)
{
    glState.clearScenes();

    auto mesh = Buffer::construct(width, height, "shaders/geometry-with-physics/mesh.vs", "shaders/geometry-with-physics/mesh.fs");
    mesh->setFormat(BufferFormat::Meshes);
    mesh->setNumOutTextures(2);

    auto sphere = Buffer::construct(width, height, "shaders/geometry-with-physics/sphere.vs", "shaders/geometry-with-physics/sphere.fs");
    sphere->setFormat(BufferFormat::Meshes);
    sphere->setNumOutTextures(2);
    sphere->setName("sphere");
    sphere->setDepthTest(true);

    auto rayMarch = Buffer::construct(width, height, "shaders/geometry-with-physics/rayMarch.vs", "shaders/geometry-with-physics/rayMarch.fs");
    rayMarch->setInputs(mesh, sphere);

    // auto smoke = Buffer::construct(width, height, "shaders/geometry-with-physics/smoke.vs", "shaders/geometry-with-physics/smoke.fs");

    glState.addScene("meshScene", rayMarch, Duration::seconds(9999));
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
