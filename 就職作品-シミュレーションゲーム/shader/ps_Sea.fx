#include	"psvscommon.fx"

uniform float2 mouse;
//uniform float2 resolution;
uniform SamplerState backbuffer;

// "Seascape" by Alexander Alekseev aka TDM - 2014
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

static const int NUM_STEPS = 32;
static const float PI = 3.1415;
static const float EPSILON = 1e-3;
static const float2x2 octave_m = float2x2(1.6, 1.2, -1.2, 1.6);

// sea
static const int ITER_GEOMETRY = 1;
static const int ITER_FRAGMENT = 8;
static const float SEA_HEIGHT = 0.3;
static const float SEA_CHOPPY = 0.8;
static const float SEA_SPEED = 0.5;
static const float SEA_FREQ = 0.16;
static const float3 SEA_BASE = float3(0,0.7, 0.82);
static const float3 SEA_WATER_COLOR = float3(0.8, 0.9, 0.6);

// math
float3x3 fromEuler(float3 ang) {
    float2 a1 = float2(sin(ang.x), cos(ang.x));
    float2 a2 = float2(sin(ang.y), cos(ang.y));
    float2 a3 = float2(sin(ang.z), cos(ang.z));
    float3x3 m;
    m[0] = float3(a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
    m[1] = float3(-a2.y * a1.x, a1.y * a2.y, a2.x);
    m[2] = float3(a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
    return m;
}
float hash(float2 p) {
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453123);
}
float noise(in float2 p) {
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);
    return -1.0 + 2.0 * lerp(lerp(hash(i + float2(0.0, 0.0)),
        hash(i + float2(1.0, 0.0)), u.x),
        lerp(hash(i + float2(0.0, 1.0)),
            hash(i + float2(1.0, 1.0)), u.x), u.y);
}
// lighting
float diffuse(float3 n, float3 l, float p) {
    return pow(abs(dot(n, l) * 0.4 + 0.6),abs(p));
}
float specular(float3 n, float3 l, float3 e, float s) {
    float nrm = (s + 8.0) / (3.1415 * 8.0);
    return pow(max(dot(reflect(e, n), l), 0.0), s) * nrm;
}

// sky
float3 getSkyColor(float3 e) {
    e.y = max(e.y, 0.0);
    float3 ret;
    ret.x = pow(1.0 - e.y, 2.0);
    ret.y = 1.0 - e.y;
    ret.z = 0.6 + (1.0 - e.y) * 0.4;
    return ret;
}

// sea
float sea_octave(float2 uv, float choppy) {
    uv += noise(uv);
    float2 wv = 1.0 - abs(sin(uv));
    float2 swv = abs(cos(uv));
    wv = lerp(wv, swv, wv);
    return pow(abs(1.0 - pow(abs(wv.x) * abs(wv.y), 0.65)),abs(choppy));
}

float map(float3 p,float sea_time) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;

    float d, h = 0.0;
    d = sea_octave((uv + sea_time) * freq, choppy);
    d += sea_octave((uv - sea_time) * freq, choppy);
    h += d * amp;
    uv.x = uv.x * octave_m._11 + uv.y * octave_m._21;
    uv.y = uv.y * octave_m._12 + uv.y * octave_m._22;

    freq *= 1.9;
    amp *= 0.22;
    choppy = lerp(choppy, 1.0, 0.2);
    return p.y - h;
}

float map_detailed(float3 p,float sea_time) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;

    float d, h = 0.0;
    for (int i = 0; i < ITER_FRAGMENT; i++) {
        d = sea_octave((uv + sea_time) * freq, choppy);
        d += sea_octave((uv - sea_time) * freq, choppy);
        h += d * amp;
        uv.x = uv.x * octave_m._11 + uv.y * octave_m._21;
        uv.y = uv.y * octave_m._12 + uv.y * octave_m._22;
        freq *= 1.9;
        amp *= 0.22;
        choppy = lerp(choppy, 1.0, 0.2);
    }
    return p.y - h;
}

float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist) {
    float fresnel = 1.0 - max(dot(n, -eye), 0.0);
    fresnel = pow(fresnel, 3.0) * 0.65;

    float3 reflected = getSkyColor(reflect(eye, n));
    float3 refraced = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;

    float3 color = lerp(refraced, reflected, fresnel);

    float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    //‚Æ‚è‚ ‚¦‚¸0‚Å–„‚ß‚é
    color.r += specular(n, l, eye, 60.0);
    color.g += specular(n, l, eye, 60.0);
    color.b += specular(n, l, eye, 60.0);

    return color;
}

// tracing
float3 getNormal(float3 p, float eps,float sea_time) {
    float3 n;
    n.y = map_detailed(p,sea_time);
    n.x = map_detailed(float3(p.x + eps, p.y, p.z),sea_time) - n.y;
    n.z = map_detailed(float3(p.x, p.y, p.z + eps),sea_time) - n.y;
    n.y = eps;
    return normalize(n);
}

float3 heightMapTracing(float3 ori, float3 dir,float3 p,float sea_time) {
    float tm = 0.0;
    float tx = 1000.0;
    float hx = map(ori + dir * tx,sea_time);
    if (hx > 0.0) return p;
    float hm = map(ori + dir * tm, sea_time);
    float tmid = 0.0;
    for (int i = 0; i < NUM_STEPS; i++) {
        tmid = lerp(tm, tx, hm / (hm - hx));
        p = ori + dir * tmid;
        float hmid = map(p, sea_time);
        if (hmid < 0.0) {
            tx = tmid;
            hx = hmid;
        }
        else {
            tm = tmid;
            hm = hmid;
        }
    }
    return p;
}

// main
float4 main(VS_OUTPUT input) : SV_Target
{
    float SEA_TIME = 0.1;

    float2 resolution;
   
    resolution.x = 1000;
    resolution.y = 1000;
    float EPSILON_NRM;

    EPSILON_NRM = 0.1 / resolution.x;
    SEA_TIME = TIME * SEA_SPEED;

    float2 uv = input.Pos.xy / resolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;
    float time = TIME / 50;
    //time = time * 0.3 + mouse.x * 0.01;

    // ray
    float3 ang = float3(0,-1,0);
    float3 ori = float3(EyePos.x,EyePos.y * 10, EyePos.z);
    float3 dir = normalize(float3(-uv.xy, -2.0));
    dir.z += length(uv) * 0.15;
    dir = mul(normalize(dir),fromEuler(ang));

    // tracing
    float3 p = heightMapTracing(ori, dir, p,SEA_TIME);
    float3 dist = p - ori;
    float3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM,SEA_TIME);
    float3 light = normalize(float3(0.0, 1.0, 0.8));

    // color
    float3 color = lerp(
        getSkyColor(dir),
        getSeaColor(p, n, light, dir, dist),
        pow(smoothstep(0.0, -0.05, dir.y), 0.3));

    // post
    //‹ó”’•”•ª‚ð0‚Å–„‚ß‚é
    float4 output;
    output.r = pow(abs(color.r),0.75);
    output.g = pow(abs(color.g),0.75);
    output.b = pow(abs(color.b),0.75);
    output.a = 1.0f;

    input.Color = output;

    input.Color.r *= AmbientColor.r;
    input.Color.g *= AmbientColor.g;
    input.Color.b *= AmbientColor.b;

    return input.Color;
}
