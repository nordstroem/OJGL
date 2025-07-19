#include "InnerSystemLab.h"
#include "music/Music.h"

using namespace ojgl;

struct Planet {
    Planet(float mass, const Vector3f& pos, const Vector3f& vel)
        : mass(mass)
        , pos(pos)
        , vel(vel)
    {
    }

    float mass;
    Vector3f pos;
    Vector3f vel;
};

struct SolarSystem {
    SolarSystem()
        : current(0)
    {
        // Earth
        planets.emplace_back(5.9736E+24f,
            Vector3f(-2.697729070933774E+07f, 1.446110081509904E+08f, 3.296076411029696E+04f),
            Vector3f(-2.977162072836500E+01f, -5.642530058471864E+00f, -9.703736651274220E-04f));

        // Sun
        planets.emplace_back(1.9891E+30f,
            Vector3f(3.708069410204890E+05f, 7.347520252301828E+04f, 3.887218910771462E+03f),
            Vector3f(-4.606127026089634E-03f, 7.772902526292513E-03f, 6.836544921898788E-05f));

        // Mercury
        planets.emplace_back(3.302E+23f,
            Vector3f(2.944795931327176E+07f, 3.673284879527312E+07f, 3.187668663728386E+05f),
            Vector3f(-4.783278118437298E+01f, 3.231351038493616E+01f, 7.036254471448057E+00f));

        // Venus
        planets.emplace_back(48.685E+23f,
            Vector3f(5.612647151655690E+07f, -9.340567851874861E+07f, -4.472777981490206E+06f),
            Vector3f(2.983438104670340E+01f, 1.782622362853692E+01f, -1.485258796109859E+00f));

        // Mars
        planets.emplace_back(6.4185E+23f,
            Vector3f(-1.487549755030171E+08f, -1.774231244334544E+08f, -1.512110244126618E+04f),
            Vector3f(1.947148471410864E+01f, -1.351350153821790E+01f, -7.652422983913167E-01f));

        // Jupiter
        planets.emplace_back(1898.13E+24f,
            Vector3f(-6.913702017940772E+08f, -4.284012308018234E+08f, 1.726313004182798E+07f),
            Vector3f(6.728865193791407E+00f, -1.050116335198787E+01f, -1.081880249745968E-01f));

        // Saturnus
        planets.emplace_back(5.68319E+26f,
            Vector3f(1.106094425383184E+09f, 8.238407435712537E+08f, -5.838019485357153E+07f),
            Vector3f(-6.298469201435907E+00f, 7.722993150215114E+00f, 1.137317446842210E-01f));

        // Uranus
        planets.emplace_back(86.8103E+24f,
            Vector3f(1.308726219550735E+09f, -2.639106654294327E+09f, -2.683702794167185E+07f),
            Vector3f(6.049730968527591E+00f, 2.709301878675839E+00f, -6.841965498253189E-02f));

        // Neptunus
        planets.emplace_back(102.41E+24f,
            Vector3f(-1.631896332892333E+09f, 4.174688369977927E+09f, -4.836543444482803E+07f),
            Vector3f(-5.092803136099079E+00f, -1.943289919580870E+00f, 1.575127744812126E-01f));

        // Moon
        planets.emplace_back(734.9E+20f,
            Vector3f(-2.687887244321885E+07f, 1.442239856855676E+08f, 3.694458400532603E+03f),
            Vector3f(-2.882461495739722E+01f, -5.368474982969579E+00f, -4.908562379039338E-02f));
    }

    ojstd::vector<Planet> planets;
    int current;

    ojstd::vector<float> getValues()
    {
        float scale = static_cast<float>(10.0 / 2E+09);
        ojstd::vector<float> values;
        for (int i = 0; i < planets.size(); i++) {
            values.emplace_back(planets[i].pos.x * scale);
            values.emplace_back(planets[i].pos.z * scale);
            values.emplace_back(planets[i].pos.y * scale);
        }

        return values;
    }

    void tick(int tick)
    {
        while (current < tick) {
            current++;
            const double G = 6.673e-20;
            const double dt = 100.0;
            for (int rep = 0; rep < 20; rep++) {
                Vector3f forces[10] = {};
                for (int i = 0; i < planets.size(); i++) {
                    for (int j = 0; j < planets.size(); j++) {
                        if (i != j) {
                            double r2 = (planets[j].pos - planets[i].pos).lenSq();
                            double a = G * planets[j].mass / r2;

                            Vector3f dir(planets[j].pos - planets[i].pos);
                            dir.normalize();
                            forces[i] += dir * static_cast<float>(a);
                        }
                    }
                }

                for (int i = 0; i < planets.size(); i++) {
                    planets[i].vel += forces[i] * dt;
                }

                for (int i = 0; i < planets.size(); i++) {
                    planets[i].pos += (planets[i].vel * dt);
                }
            }
        }
    }

    float getMarsScale()
    {
        const float thres = 1000 * 7;
        if (current > thres) {
            planets[4].mass = 1.9891E+30f;
            float f = (current - thres) / 1000;
            if (f > 1.0f) {
                f = 1.0f;
            }
            return 1.0f + f * 200.0f;
        }
        return 1.0f;
    }
};

ojstd::string InnerSystemLab::getTitle() const
{
    return "OJ - Inner System Lab";
}

static const unsigned char song[] = {
#include "songs/innersystemlab_song.inc"
};

const unsigned char* InnerSystemLab::getSong() const
{
    return song;
}

#define TIME_1 58
#define TIME_2 47
#define TIME_3 25
#define TIME_4 24

static SolarSystem solarSystem1;
static SolarSystem solarSystem2;

ojstd::vector<Scene> InnerSystemLab::buildSceneGraph(const Vector2i& sceneSize) const
{

    ojstd::vector<Scene> scenes;

    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_1.fs");
        edison->setUniformCallback([](float relativeSceneTime) -> Buffer::UniformVector {
          solarSystem1.tick(ojstd::ftoi(relativeSceneTime * 1000));
          return { ojstd::make_shared<Uniform3fv>("planets", solarSystem1.getValues()) };
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(edison);
        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_1_post.fs");
        post->setInputs(fxaa);
        scenes.emplace_back(post, Duration::seconds(TIME_1));
    }
    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_2.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(edison);
        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_1_post.fs");
        post->setInputs(fxaa);
        scenes.emplace_back(post, Duration::seconds(TIME_2));
    }

    {
        auto planets = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_planets.fs");
        planets->setUniformCallback([](float relativeSceneTime) {
            Buffer::UniformVector vector;
            solarSystem2.tick(ojstd::ftoi(relativeSceneTime * 1000));

            vector.push_back(ojstd::make_shared<Uniform3fv>("planets", solarSystem2.getValues()));
            vector.push_back(ojstd::make_shared<Uniform1f>("marsScale", solarSystem2.getMarsScale()));

            return vector;
        });
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(planets);
        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_1_post.fs");
        post->setInputs(fxaa);
        scenes.emplace_back(post, Duration::seconds(TIME_3));
    }

    {
        auto hospital = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/hospital.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(hospital);
        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "innersystemlab/edison_1_post.fs");
        post->setInputs(fxaa);
        scenes.emplace_back(post, Duration::seconds(TIME_4));
    }

    return scenes;
}