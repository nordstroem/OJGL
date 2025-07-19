#include "QED.h"
#include "FreeCameraController.h"

using namespace ojgl;

static constexpr int NUM_SCENES = 6;
static float sceneLengths[NUM_SCENES] = { 20., 10., 5., 5., 9, 14. };

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
    return 99999999999.f;
}

static auto generateRandomMatrices(int number)
{
    ojstd::vector<Matrix> matrices;
    for (int i = 0; i < number; i++)
        matrices.push_back(Matrix::randomRotation());
    return matrices;
}

class SphereMeshCallback {
public:
    ojstd::vector<ojstd::Pair<ojstd::shared_ptr<Mesh>, Matrix>> operator()(float relativeSceneTime)
    {
        auto& cameraController = FreeCameraController::instance();

        // Handle sphere scene
        ojstd::vector<ojstd::Pair<ojstd::shared_ptr<Mesh>, Matrix>> meshList;

        float baseTime = relativeSceneTime;
        int cs = currentSubScene(baseTime);
        float lTime = localSubTime(baseTime);
        float lTimeLeft = localSubTimeLeft(baseTime);

        float to = 3.f;
        if (cs == 4 && lTime > 3) {
            float k = 2.f;
            auto [fraction, base] = ojstd::modf((lTime - to) / k);
            float time = k * fraction;
            float gf = 0.2f;
            float v0 = 16;
            float beta = 0.f;
            float alpha = 0.26f;
            spherePosition.x = 0.8f + v0 * ojstd::cos(alpha) * ojstd::cos(beta) * time;
            spherePosition.z = v0 * ojstd::cos(alpha) * ojstd::sin(beta) * time;
            spherePosition.y = 0.2f + v0 * ojstd::sin(alpha) * time - time * time * gf;
            meshList.emplace_back(sphere, Matrix::translation(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix::scaling(0.02f));
        }

        if (cs == 5) {
            float k = 2.f;
            auto [fraction, base] = ojstd::modf((lTime - to) / k);
            //float time = lTimeLeft > 6 ? k * fraction : lTime - to;
            float time = lTime;
            float gf = 0.0;

            int num = 320;
            for (int i = 0; i < num; i++) {
                float v0 = 5.f;
                float beta = 0 + (i - num / 2) / 580.f;
                float alpha = 0.26f + 0.2f * ojstd::sin(1.f * i + time);

                float x = 0.5f + v0 * ojstd::cos(alpha) * ojstd::cos(beta) * time;
                float y = 0.2f + v0 * ojstd::sin(alpha) * time - time * time * gf;
                float z = v0 * ojstd::cos(alpha) * ojstd::sin(beta) * time;

                int numForQ = 100;
                int numForE = 100;
                int numForD = 100;
                float qX = 25;
                float qY = 0;
                float qZ = 0;

                if (i < numForQ) {
                    int numForCircle = 85;
                    int numForLine = numForQ - numForCircle;
                    float r = 3;
                    float tau = 2.f * ojstd::pi;
                    qY = 8 + r * ojstd::cos(tau * i / (numForCircle - 1));
                    qZ = r * ojstd::sin(tau * i / (numForCircle - 1));

                    if (i >= numForCircle) {
                        float st = 1.f * (i - numForCircle) / (numForLine - 1);
                        qY = ojstd::lerp(7, 5, st);
                        qZ = ojstd::lerp(1, 3, st);
                    }
                    qZ -= 6.5;
                } else if (i < (numForQ + numForE)) {
                    int numLine1 = 32;
                    int numLine2 = 25;
                    int numLine3 = 18;
                    int numLine4 = 25;
                    int j = i - numForQ;

                    if (j < numLine1) {
                        float st = 1.f * j / (numLine1 - 1);
                        qY = ojstd::lerp(11, 5, st);
                        qZ = 0;
                    } else if (j < (numLine1 + numLine2)) {
                        float st = 1.f * (j - numLine1) / (numLine2 - 1);
                        qY = 11;
                        qZ = ojstd::lerp(0, 3, st);
                    } else if (j < (numLine1 + numLine2 + numLine3)) {
                        float st = 1.f * (j - numLine1 - numLine2) / (numLine3 - 1);
                        qY = 8;
                        qZ = ojstd::lerp(0, 2, st);
                    } else {
                        float st = 1.f * (j - numLine1 - numLine2 - numLine3) / (numLine4 - 1);
                        qY = 5;
                        qZ = ojstd::lerp(0, 3, st);
                    }
                    qZ -= 0.5;
                } else if (i < (numForQ + numForE + numForD)) {
                    int j = i - numForQ - numForE;
                    int numForCircle = 50;
                    int numForLine1 = 30;
                    int numForLine2 = 10;
                    int numForLine3 = 10;
                    float r = 3;
                    float tau = 2.f * ojstd::pi;
                    qY = 8 + r * ojstd::cos(tau / 2 * j / (numForCircle - 1));
                    qZ = 1.5f + r * ojstd::sin(tau / 2 * j / (numForCircle - 1));

                    if (j >= numForCircle && j < numForCircle + numForLine1) {
                        float st = 1.f * (j - numForCircle) / (numForLine1 - 1);
                        qY = ojstd::lerp(11, 5, st);
                        qZ = 0;
                    } else if (j >= numForCircle && j < numForCircle + numForLine1 + numForLine2) {
                        float st = 1.f * (j - numForCircle - numForLine1) / (numForLine2 - 1);
                        qY = 11;
                        qZ = ojstd::lerp(0, 1.5, st);
                    } else if (j >= numForCircle && j < numForCircle + numForLine1 + numForLine2 + numForLine3) {
                        float st = 1.f * (j - numForCircle - numForLine1 - numForLine2) / (numForLine3 - 1);
                        qY = 5;
                        qZ = ojstd::lerp(0, 1.5, st);
                    }
                    qZ += 5.3f;
                } else {
                    int numForCircle = 10;
                    int j = i - numForQ - numForE - numForD;
                    float r = 0.1f;
                    float tau = 2.f * ojstd::pi;
                    qY = 5 + r * ojstd::cos(tau * i / (numForCircle - 1));
                    qZ = r * ojstd::sin(tau * i / (numForCircle - 1));
                    qZ -= j > numForCircle ? -3.8f : 2.f;
                }
                qY -= 1.6f + (ojstd::hash1(static_cast<float>(i)) - 0.5f) * 0.2f + ojstd::sin(lTime * 1.f + static_cast<float>(i)) * 0.02f;
                qZ += -0.75f + (ojstd::hash1(static_cast<float>(i)) - 0.5f) * 0.2f + ojstd::sin(lTime * 2.f + static_cast<float>(i)) * 0.05f;
                float st = ojstd::smoothstep(2.f, 7.f, lTime);
                spherePosition.x = ojstd::lerp(x, qX, st);
                spherePosition.y = ojstd::lerp(y, qY, st);
                spherePosition.z = ojstd::lerp(z, qZ, st);
                //32 8 (-10, 10)
                //LOG_INFO(spherePosition.x << ", " << spherePosition.y << ", " << spherePosition.z);
                meshList.emplace_back(sphere, Matrix::translation(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix::scaling(0.01f));
            }
        }

        if (cs == 0 && prevCs != cs)
            // cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
            cameraController.set({ 2.57f, -0.375f, 7.71f }, 0.36f, 0.08f);

        if (cs == 2) {
            float f = ojstd::smoothstep(0.f, 2.f, lTime);
            float xPos = ojstd::lerp(3.3f, -1.27f, f);
            float heading = ojstd::lerp(0.36f, -0.18f, f);
            cameraController.set({ xPos, 5.67f, 8.5f }, heading, -0.6f);
            //      cameraController.set({ 3.3, 5.67, 8.5 }, 0.36, -0.6);
            //     cameraController.set({ -1.27, 5.67, 8.66 }, -0.18, -0.6);
        }
        //    cameraController.set({ -1.27, 5.67, 8.66 }, -0.18, -0.6);

        if (cs == 0 || cs == 1) {
            float r = 4;
            float w = 1.;
            float sphereX = r * ojstd::cos(baseTime * w);
            float sphereZ = r * ojstd::sin(baseTime * w);
            float sc = ojstd::smoothstep(10.f, 12.f, baseTime) * 0.02f;
            meshList.emplace_back(sphere, Matrix::translation(sphereX, 0, sphereZ) * Matrix::scaling(sc));

            if (lTimeLeft < 3. && cs == 0) {
                cameraController.set({ 3.3f, 5.67f, 8.5f }, 0.36f, -0.6f);
            }
        }

        if (cs == 1 || cs == 2 || cs == 3 || cs == 4) {
            for (auto& m : randomMatrices) {
                float r = 7;
                float xo = 0.;
                float yo = 0.;
                if (cs == 4) {
                    r = 7.f * (1.f - ojstd::smoothstep(1.f, 0.f, lTimeLeft));
                    xo = ojstd::lerp(0.f, 0.8f, ojstd::smoothstep(1.f, 0.f, lTimeLeft));
                    yo = ojstd::lerp(0.f, 0.2f, ojstd::smoothstep(1.f, 0.f, lTimeLeft));
                }
                float w = 0.1f;
                float sphereX = r * (ojstd::cos(baseTime * w));
                float sphereZ = r * ojstd::sin(baseTime * w);
                float sc = cs >= 2.f ? 0.01f : ojstd::smoothstep(0.f, 5.f, lTime) * 0.01f;
                sc = cs == 4.f ? (0.005f + (1.0f - ojstd::smoothstep(2.0f, 0.f, lTimeLeft)) * 0.005f) : sc;
                meshList.emplace_back(sphere, Matrix::translation(xo, yo, 0.f) * m * Matrix::translation(sphereX, 0, sphereZ) * Matrix::scaling(sc));
            }
        }

        if (cs == 3 && prevCs != cs) {
            cameraController.set({ -2.71f, 0.79f, 4.99f }, -0.64f, -0.22f);
        }

        if (cs == 4 && lTimeLeft < 2.5)
            cameraController.set({ 12.95f, 4.21f, 9.4f }, 0.8f, -0.2f);

        if (cs == 5) {
            float f = ojstd::smoothstep(13.f, 8.5f, lTimeLeft);
            float xPos = ojstd::lerp(12.95f, 12.63f, f);
            float yPos = ojstd::lerp(4.21f, 6.21f, f);
            float zPos = ojstd::lerp(9.4f, -0.6123f, f);
            float heading = ojstd::lerp(0.8f, -1.58f, f);
            float elevation = ojstd::lerp(-0.2f, 0.0f, f);
            cameraController.set({ xPos, yPos, zPos }, heading, elevation);

            //  (12.6313, 4.21, -0.61297), [ -1.58, 0.18 ]
        }

        prevCs = cs;

        return meshList;
    }

private:
    ojstd::vector<Matrix> randomMatrices = generateRandomMatrices(500);
    int prevCs = -1;
    ojstd::shared_ptr<Mesh> sphere = Mesh::constructCube();
    Vector3f spherePosition;
    float previousTime = 0.f;
};

ojstd::vector<Scene> QED::buildSceneGraph(const Vector2i& sceneSize) const
{
    auto& cameraController = FreeCameraController::instance();
    Matrix cameraMatrix = cameraController.getCameraMatrix();
    Matrix cameraMatrixInverse = cameraMatrix.inverse();

    constexpr float fov = 0.927295218f;
    float aspectRatio = static_cast<float>(sceneSize.x) / static_cast<float>(sceneSize.y);

    ojstd::vector<Scene> scenes;
    {
        auto tower = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/tower.fs");

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur1->setInputs(tower);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        auto towerPost = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/post.fs");
        towerPost->setInputs(blur2);

        scenes.emplace_back(towerPost, Duration::seconds(8 + 15 + 17 + 12));
    }

    {
        auto sphere = Buffer::construct(sceneSize.x, sceneSize.y, "QED/sphere.vs", "QED/sphere.fs");
        sphere->setFormat(BufferFormat::Meshes);
        sphere->setNumOutTextures(2);
        sphere->setDepthTest(true);

        sphere->setUniformCallback([&cameraController, aspectRatio, fov]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            Matrix cameraMatrix = cameraController.getCameraMatrix();
            Matrix cameraMatrixInverse = cameraMatrix.inverse();
            return { ojstd::make_shared<UniformMatrix4fv>("P", Matrix::perspective(fov, aspectRatio, 0.001f, 100.0f) * cameraMatrixInverse) };
        });

        sphere->setMeshCallback(SphereMeshCallback {});

        auto rayMarch = Buffer::construct(sceneSize.x, sceneSize.y, "QED/rayMarch.vs", "QED/rayMarch.fs");
        rayMarch->setInputs(sphere);

        rayMarch->setUniformCallback([&cameraController]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            Matrix cameraMatrix = cameraController.getCameraMatrix();
            return {
                ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", cameraMatrix),
            };
        });

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur1->setInputs(rayMarch);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/post.fs");
        post->setInputs(blur2);

        scenes.emplace_back(post, Duration::seconds(63));
    }

    return scenes;
}

ojstd::string QED::getTitle() const
{
    return "OJ - Q.E.D.";
}

static const unsigned char song[] = {
#include "songs/qed_song.inc"
};

const unsigned char* QED::getSong() const {
  return song;
}
