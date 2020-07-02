
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

static constexpr int NUM_SCENES = 6;
static float sceneLengths[NUM_SCENES] = { 20., 10., 5., 5., 9, 13. };

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

    float to = 3.f;
    if (cs == 4 && lTime > 3) {
        float k = 2.f;
        auto [fraction, base] = ojstd::modf((lTime - to) / k);
        float time = k * fraction;
        float gf = 0.2;
        float v0 = 16;
        float beta = 0.f;
        float alpha = 0.26f;
        spherePosition.x = 0.8f + v0 * ojstd::cos(alpha) * ojstd::cos(beta) * time;
        spherePosition.z = v0 * ojstd::cos(alpha) * ojstd::sin(beta) * time;
        spherePosition.y = 0.2f + v0 * ojstd::sin(alpha) * time - time * time * gf;
        state["meshScene"]["sphere"].insertMesh(sphere, Matrix::translation(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix::scaling(0.02f));
    }

    if (cs == 5) {
        float k = 2.f;
        auto [fraction, base] = ojstd::modf((lTime - to) / k);
        //float time = lTimeLeft > 6 ? k * fraction : lTime - to;
        float time = lTime;
        float gf = 0.0;

        int num = 500;
        for (int i = 0; i < num; i++) {
            float v0 = 5.f;
            float beta = 0 + (i - num / 2) / 580.f;
            float alpha = 0.26f + 0.2 * ojstd::sin(1.f * i + time);

            spherePosition.x = 0.5f + v0 * ojstd::cos(alpha) * ojstd::cos(beta) * time;
            spherePosition.z = v0 * ojstd::cos(alpha) * ojstd::sin(beta) * time;
            spherePosition.y = 0.2f + v0 * ojstd::sin(alpha) * time - time * time * gf;
            state["meshScene"]["sphere"].insertMesh(sphere, Matrix::translation(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix::scaling(0.01f));
        }
    }

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

        if (lTimeLeft < 3. && cs == 0) {
            cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
        }
    }

    if (cs == 1 || cs == 2 || cs == 3 || cs == 4) {
        for (auto& m : randomMatrices) {
            float r = 7;
            float xo = 0.;
            float yo = 0.;
            if (cs == 4) {
                r = 7.f * (1.f - ojstd::smoothstep(1.f, 0.f, lTimeLeft));
                xo = ojstd::lerp(0, 0.8, ojstd::smoothstep(1.f, 0.f, lTimeLeft));
                yo = ojstd::lerp(0, 0.2, ojstd::smoothstep(1.f, 0.f, lTimeLeft));
            }
            float w = 0.1;
            float sphereX = r * (ojstd::cos(baseTime * w));
            float sphereZ = r * ojstd::sin(baseTime * w);
            float sc = cs >= 2 ? 0.01 : ojstd::smoothstep(0, 5, lTime) * 0.01;
            sc = cs == 4 ? (0.005 + (1.0 - ojstd::smoothstep(2.0, 0., lTimeLeft)) * 0.005) : sc;
            state["meshScene"]["sphere"].insertMesh(sphere, Matrix::translation(xo, yo, 0) * m * Matrix::translation(sphereX, 0, sphereZ) * Matrix::scaling(sc));
        }
    }

    if (cs == 3 && prevCs != cs) {
        cameraController.set({ -2.71, 0.79, 4.99 }, -0.64, -0.22);
    }

    if (cs == 4 && lTimeLeft < 2.5)
        cameraController.set({ 12.95, 4.21, 9.4 }, 0.8, -0.2);

    if (cs == 5) {
        float f = ojstd::smoothstep(12., 5., lTimeLeft);
        float xPos = ojstd::lerp(12.95, 12.63, f);
        float yPos = ojstd::lerp(4.21, 4.21, f);
        float zPos = ojstd::lerp(9.4, -0.6123, f);
        float heading = ojstd::lerp(0.8, -1.58, f);
        float elevation = ojstd::lerp(-0.2, 0.25, f);
        cameraController.set({ xPos, yPos, zPos }, heading, elevation);

        //  (12.6313, 4.21, -0.61297), [ -1.58, 0.18 ]
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

    ShaderReader::setBasePath("examples/shaders/");
    ShaderReader::preLoad("common/noise.fs", resources::fragment::noise);
    ShaderReader::preLoad("common/primitives.fs", resources::fragment::primitives);
    ShaderReader::preLoad("common/raymarch_utils.fs", resources::fragment::raymarch_utils);
    ShaderReader::preLoad("common/utils.fs", resources::fragment::utils);
    ShaderReader::preLoad("common/quad.vs", resources::vertex::quad);
    ShaderReader::preLoad("geometry-with-physics/mesh.fs", resources::fragment::mesh);
    ShaderReader::preLoad("geometry-with-physics/mesh.vs", resources::vertex::mesh);
    ShaderReader::preLoad("geometry-with-physics/rayMarch.fs", resources::fragment::rayMarch);
    ShaderReader::preLoad("geometry-with-physics/rayMarch.vs", resources::vertex::rayMarch);
    ShaderReader::preLoad("geometry-with-physics/sphere.fs", resources::fragment::sphere);
    ShaderReader::preLoad("geometry-with-physics/sphere.vs", resources::vertex::sphere);
    ShaderReader::preLoad("geometry-with-physics/blur1.fs", resources::fragment::blur1);
    ShaderReader::preLoad("geometry-with-physics/blur2.fs", resources::fragment::blur2);
    ShaderReader::preLoad("geometry-with-physics/post.fs", resources::fragment::post);

    ShaderReader::preLoad("fibber-reborn/raymarch_utils.fs", resources::fragment::fibberReborn::raymarchUtils);
    ShaderReader::preLoad("fibber-reborn/tower.fs", resources::fragment::fibberReborn::tower);
    ShaderReader::preLoad("fibber-reborn/tower_final.fs", resources::fragment::fibberReborn::towerFinal);

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
#ifdef _DEBUG
        cameraController.update(window);
#endif
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
        debugTitle.append(" scene time: ");
        debugTitle.append(ojstd::to_string(static_cast<int>(localSubTimeLeft(glState.relativeSceneTime().toSeconds()))));
        debugTitle.append(" subscene: ");
        debugTitle.append(ojstd::to_string(static_cast<int>(currentSubScene(glState.relativeSceneTime().toSeconds()))));

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
    if (false) {
        auto tower = Buffer::construct(width, height, "common/quad.vs", "fibber-reborn/tower.fs");

        auto blur1 = Buffer::construct(width, height, "common/quad.vs", "geometry-with-physics/blur1.fs");
        blur1->setInputs(tower);

        auto blur2 = Buffer::construct(width, height, "common/quad.vs", "geometry-with-physics/blur2.fs");
        blur2->setInputs(blur1);

        auto towerPost = Buffer::construct(width, height, "common/quad.vs", "fibber-reborn/tower_final.fs");
        towerPost->setInputs(blur2);

        glState.addScene("towerScene", blur2, Duration::seconds(8 + 15 + 17 + 12));
    }

    {
        auto mesh = Buffer::construct(width, height, "geometry-with-physics/mesh.vs", "geometry-with-physics/mesh.fs");
        mesh->setFormat(BufferFormat::Meshes);
        mesh->setNumOutTextures(2);

        auto sphere = Buffer::construct(width, height, "geometry-with-physics/sphere.vs", "geometry-with-physics/sphere.fs");
        sphere->setFormat(BufferFormat::Meshes);
        sphere->setNumOutTextures(2);
        sphere->setName("sphere");
        sphere->setDepthTest(true);

        auto rayMarch = Buffer::construct(width, height, "geometry-with-physics/rayMarch.vs", "geometry-with-physics/rayMarch.fs");
        rayMarch->setInputs(mesh, sphere);

        auto blur1 = Buffer::construct(width, height, "common/quad.vs", "geometry-with-physics/blur1.fs");
        blur1->setInputs(rayMarch);

        auto blur2 = Buffer::construct(width, height, "common/quad.vs", "geometry-with-physics/blur2.fs");
        blur2->setInputs(blur1);

        auto post = Buffer::construct(width, height, "common/quad.vs", "geometry-with-physics/post.fs");
        post->setInputs(blur2);

        glState.addScene("meshScene", post, Duration::seconds(62));
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
