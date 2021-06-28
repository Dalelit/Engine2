// Voronoi noise from https://www.youtube.com/watch?v=l-07BXzNdPw
// Confirming some ideas from https://medium.com/@jannik_boysen/procedural-skybox-shader-137f6b0cb77c

TextureCube tex : register(t0);
SamplerState sam : register(s0);

static const int MAX_STEPS = 100;
static const float FLT_MAX = 3.402823466e+38;
static const float EPSILON = 0.001;
static const float MAX_DIST = 10000.0;
static const float SURFACE_DIST = 0.01;

static const float PI = 3.141592654;
static const float TWOPI = PI * 2.0;
static const float HALFPI = PI * 0.5;

static const float4 skyColor = float4(0.1, 0.1, 0.9, 1.0);
static const float4 horizonColor = float4(0.2, 0.2, 0.2, 1.0);
static const float4 groundColor = float4(0.1, 0.3, 0.1, 1.0);
static const float groundHorizonEmphasis = 1.03;
static const float skyFogPower = 200;
static const float groundFogPower = 400;

float2 Noise22(float2 p)
{
    float3 a = frac(p.xyx * float3(123.34, 234.34, 345.65));
    a += dot(a, a + 34.45);
    return frac(float2(a.x * a.y, a.y * a.z));
}

struct Sphere {
    float3 centre;
    float radius;
    float3 color;
};

float GetDistSphere(float3 p, Sphere s)
{
    return length(p - s.centre) - s.radius;
}

static const Sphere sun = { float3(30.0, 30.0, 30.0), 5.0, float3(1.0, 1.0, 1.0) };
static const Sphere planets[3] = {
    { float3(-20.0, 10.0, 20.0), 3.5, float3(0.8, 0.2, 0.2) },
    { float3(0.0, 5.0, 15.0), 2.0, float3(0.2, 0.8, 0.2) },
    { float3(0.0, 1.9, -18.0), 4.4, float3(0.2, 0.2, 0.8) },
};

float GetDistScene(float3 p, out int index)
{
    float dist = GetDistSphere(p, sun);
    index = -1;

    for (int i = 0; i < 3; i++)
    {
        float distSurf = GetDistSphere(p, planets[i]);
        if (distSurf < dist)
        {
            index = i;
            dist = distSurf;
        }
    }

    return dist;
}

float RayMarch(float3 orig, float3 dir, out int index)
{
    float dist = 0.0;

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 p = orig + dir * dist;

        float dSurf = GetDistScene(p, index);

        dist += dSurf;

        if (dist >= MAX_DIST || dSurf < SURFACE_DIST) break;
    }

    return dist;
}

float4 CalcColor(float3 orig, float3 dir, float dist, int index)
{
    float3 color = planets[index].color;

    float3 normal = normalize((orig + dist * dir) - planets[index].centre);
    float3 toSun = normalize(sun.centre - planets[index].centre);

    color *= dot(normal, toSun);

    return float4(color, 1.0);
}

float4 VoronoiStarField(float yaw, float pitch)
{
    float val = 0.0;

    // tiling
    yaw = frac(yaw * 8.0);
    //pitch = frac(pitch * 2.0);

    for (float i = 0.0; i < 150.0; i++)
    {
        float2 noise = Noise22(float2(i, i));
        float2 p = sin(noise * TWOPI) * 0.5 + 0.5;
        //float2 p = noise;
        float d = length(float2(yaw, pitch) - p);
        val += smoothstep(0.002, 0.001, d);
    }

    return float4(val, val, val, 1.0);
}

float4 VoronoiStarField2(float yaw, float pitch)
{
    float val = 0.0;

    float2 uv = float2(yaw, pitch) * float2(41.0, 35.0);
    float2 griduv = frac(uv) - 0.5;
    float2 cellId = floor(uv);

    for (float y = -1.0; y <= 1.0; y++)
    {
        for (float x = -1.0; x <= 1.0; x++)
        {
            float2 offset = float2(x, y);
            float2 noise = Noise22(cellId + offset);
            float2 p = offset + sin(noise * TWOPI) * 0.5 + 0.5;

            float d = length(griduv - p);
            val += smoothstep(0.02, 0.001, d);
        }
    }

    return float4(val, val, val, 1.0);
}

float4 main(float3 posWS : WSPosition) : SV_TARGET
{
    //pixel = float4((posWS + 1.0) * 0.5, 1.0);    // rgb color
    //pixel = tex.Sample(sam, posWS);              // texture

    float3 orig = float3(0.0, 0.0, 0.0);
    float3 dir = normalize(posWS);

    float angle = asin(abs(dir.y)) * 0.5; // angle from the horizon
    float bearing = (atan2(dir.x, dir.z) / PI) * 0.5 + 0.5; // bearing around the horizon

    int index;

    float4 pixel = float4(0.0, 0.0, 0.0, 1.0);

    float skyRatio = smoothstep(0.0, PI / 2.0, angle); // fit to 0-1 range
    float fog = (1.0 - skyRatio);

    if (posWS.y > 0.0) // above the horizon
    {
        float dist = RayMarch(orig, dir, index);

        if (dist < MAX_DIST) // hit an object, so use it's color
        {
            if (index == -1) pixel = float4(sun.color, 1.0);
            else if (index >= 0) pixel = CalcColor(orig, dir, dist, index);
        }
        else
        {
            //pixel = skyColor; // or could sample texture
            pixel = VoronoiStarField2(bearing, angle);
        }
        pixel = max(pixel, lerp(float4(0.0, 0.0, 0.0, 1.0), horizonColor, pow(fog, skyFogPower))); // take the max value of either the object or the horizon.
        //pixel = lerp(pixel, horizonColor, pow(fog, skyFogPower)); // take the max value of either the object or the horizon.

    }
    else // ground
    {
        pixel = groundColor; // or could sample texture
        pixel = lerp(pixel, horizonColor * groundHorizonEmphasis, pow(fog, groundFogPower));
    }
    
    return pixel;
}
