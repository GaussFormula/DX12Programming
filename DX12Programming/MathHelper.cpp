#include "MathHelper.h"
#include <random>
#include <time.h>
#include <limits>

static std::default_random_engine ourRandomEngine(time(0));
static std::uniform_real_distribution<float> ourDistribution(0, 1);

const float MathHelper::Infinity = std::numeric_limits<float>::max();
const float MathHelper::Pi = 3.1415926535f;

// Returns random float in [0,1).
float MathHelper::RandF()
{
    return ourDistribution(ourRandomEngine);
}

// Returns random float in [a,b).
float MathHelper::RandF(float a, float b)
{
    return a + RandF() * (b - a);
}

int MathHelper::Rand(int a, int b)
{
    return a + (int)(RandF()) % ((b - a) + 1);
}

float MathHelper::AngleFromXY(float x, float y)
{
    float theta = 0.0f;

    // Quadrant for I and IV
    if (x >= 0.0f)
    {
        theta = atan2f(y, x); // in[-pi/2, +pi/2]
        if (theta < 0.0f)
        {
            theta += 2.0f * Pi;
        }
    }
    else
    {
        theta = atan2f(y, x) + Pi;
    }
}

DirectX::XMVECTOR MathHelper::RandUnitVec3()
{
    using namespace DirectX;
    XMVECTOR One = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
    XMVECTOR Zero = DirectX::XMVectorZero();

    // Keep trying until we get a point on/in the hemisphere.
    while (true)
    {
        // Generate random point in the cube [-1,1]^3.
        XMVECTOR v = DirectX::XMVectorSet(MathHelper::RandF(-1.0f, 1.0f),
            MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 
            0.0f);

        // Ignore points outside the unit sphere in order to get an even distribution 
        // over the unit sphere.  Otherwise points will clump more on the sphere near 
        // the corners of the cube.
        if (XMVector3Greater(XMVector3LengthSq(v), One))
        {
            continue;
        }
        return XMVector3Normalize(v);
    }
}

DirectX::XMVECTOR MathHelper::RandHemisphereUnitVec3(DirectX::XMVECTOR n)
{
    using namespace DirectX;
    XMVECTOR One = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
    XMVECTOR Zero = DirectX::XMVectorZero();

    while (true)
    {
        // Generate random point in the cube [-1,1]^3.
        XMVECTOR v = DirectX::XMVectorSet(MathHelper::RandF(-1.0f, 1.0f),
            MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f),
            0.0f);

        if (DirectX::XMVector3Greater(DirectX::XMVector3LengthSq(v), One))
        {
            continue;
        }

        // Ignore points in the bottom hemisphere.
        if (DirectX::XMVector3Less(XMVector3Dot(n, v), Zero))
        {
            continue;
        }

        return XMVector3Normalize(v);
    }
}