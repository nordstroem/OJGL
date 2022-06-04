#pragma once
#include "Vector.h"
#include "utility/OJstd.h"

namespace ojgl {

// a0 + a1 * x + a2 * x^2 + a3 * x^3
struct Polynomial3 {
    float a0 = 0.f;
    float a1 = 0.f;
    float a2 = 0.f;
    float a3 = 0.f;
};

template <size_t N>
ojstd::vector<Polynomial3> polynomialLoad(float (&values)[N])
{
    ojstd::vector<Polynomial3> polynomials;
    // Reserve?
    for (auto i = 0u; i < N; i += 4) {
        polynomials.push_back(Polynomial3 { values[i], values[i + 1], values[i + 2], values[i + 3] });
    }

    return polynomials;
}

class Spline3 {

public:
    Spline3(const ojstd::vector<float>& ts, const ojstd::vector<Polynomial3>& xParameters, const ojstd::vector<Polynomial3>& yParameters, const ojstd::vector<Polynomial3>& zParameters)
        : _ts(ts)
        , _xParameters(xParameters)
        , _yParameters(yParameters)
        , _zParameters(zParameters)
    {
    }

    Vector3f operator()(float t) const
    {
        return { apply(t, _xParameters), apply(t, _yParameters), apply(t, _zParameters) };
    }

private:
    float apply(float t, const ojstd::vector<Polynomial3>& params) const
    {
        int indx = ojstd::clamp(ojstd::bisect_left(_ts, t) - 1, 0, _ts.size() - 2);
        float t0 = _ts[indx];
        const Polynomial3& p = params[indx];
        float to = t - t0;
        return p.a0 + p.a1 * to + p.a2 * to * to + p.a3 * to * to * to;
    }

    ojstd::vector<float> _ts;
    ojstd::vector<Polynomial3> _xParameters;
    ojstd::vector<Polynomial3> _yParameters;
    ojstd::vector<Polynomial3> _zParameters;
};

}
