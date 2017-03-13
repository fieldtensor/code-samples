attribute vec4 re_Position;
attribute vec4 p;
attribute vec4 q;

uniform mat4 re_WorldMatrix;
uniform mat4 re_MVPMatrix;
uniform mat4 re_VPMatrix;

uniform float lapFactor;

uniform float time;

varying vec3 re_FragPosition;
varying vec3 re_FragNormal;

const int WCOUNT = 8;

uniform float A[WCOUNT];
uniform vec2 k[WCOUNT];
uniform float w[WCOUNT];

uniform vec3 re_LightDirection;
uniform vec3 re_Eye;
uniform sampler2D depthTex;

uniform vec3 depthP1;
uniform vec3 depthP2;

uniform float depthMin;
uniform float depthMax;

uniform bool rflat;

varying vec4 re_FragColor;

const float R0 = 0.625;

/*
vec4 sv = re_WorldMatrix * vec4(re_Position.xy, 0.0, 1.0);

float s = (sv.x - depthP1.x) / (depthP2.x - depthP1.x);
float t = (sv.z - depthP1.z) / (depthP2.z - depthP1.z);

vec4 tv = texture2D(depthTex, vec2(s, t));
float q = (tv.r + tv.g + tv.b + tv.a) / 4.0;
*/

void
main(void)
{    
    float h1 = 0.0;
    float h2 = 0.0;
    float h3 = 0.0;

    float g = 9.8;
    float pi = 3.14159265;

    float t = time / 2.5;

    float AF = 1.0;
    float DF = 1.0;

    vec3 wp1 = vec3(re_WorldMatrix * vec4(re_Position.xy, 0.0, 1.0));
    vec3 wp2 = vec3(re_WorldMatrix * vec4(p.xy, 0.0, 1.0));
    vec3 wp3 = vec3(re_WorldMatrix * vec4(q.xy, 0.0, 1.0));

    float baseline = 0.06;

    if( ! rflat )
    {
        for(int i = 0; i < WCOUNT; i++)
        {
            float phi = dot(k[i], DF * wp1.xz) - w[i] * t;
            h1 += AF * A[i] * sin(phi);
        }

        for(int i = 0; i < WCOUNT; i++)
        {
            float phi = dot(k[i], DF * wp2.xz) - w[i] * t;
            h2 += AF * A[i] * sin(phi);
        }

        for(int i = 0; i < WCOUNT; i++)
        {
            float phi = dot(k[i], DF * wp3.xz) - w[i] * t;
            h3 += AF * A[i] * sin(phi);
        }

        float u1 = (wp1.x - depthP1.x) / (depthP2.x - depthP1.x);
        float u2 = (wp2.x - depthP1.x) / (depthP2.x - depthP1.x);
        float u3 = (wp3.x - depthP1.x) / (depthP2.x - depthP1.x);

        float v1 = 1.0 - (wp1.z - depthP1.z) / (depthP2.z - depthP1.z);
        float v2 = 1.0 - (wp2.z - depthP1.z) / (depthP2.z - depthP1.z);
        float v3 = 1.0 - (wp3.z - depthP1.z) / (depthP2.z - depthP1.z);

        vec4 tv1 = texture2D(depthTex, vec2(u1, v1));
        vec4 tv2 = texture2D(depthTex, vec2(u2, v2));
        vec4 tv3 = texture2D(depthTex, vec2(u3, v3));

        float tvs1 = (tv1.r + tv1.g + tv1.b + tv1.a) / 4.0;
        float tvs2 = (tv2.r + tv2.g + tv2.b + tv2.a) / 4.0;
        float tvs3 = (tv3.r + tv3.g + tv3.b + tv3.a) / 4.0;

        float depth1 = baseline - (depthMin + tvs1 * (depthMax - depthMin));
        float depth2 = baseline - (depthMin + tvs2 * (depthMax - depthMin));
        float depth3 = baseline - (depthMin + tvs3 * (depthMax - depthMin));

        float rf = 0.36 + baseline;
        float r1 = clamp(depth1 / rf, 0.0, 1.0);
        float r2 = clamp(depth2 / rf, 0.0, 1.0);
        float r3 = clamp(depth3 / rf, 0.0, 1.0);

        wp1.y += h1 * (lapFactor + r1 * (1.0 - lapFactor));
        wp2.y += h2 * (lapFactor + r2 * (1.0 - lapFactor));
        wp3.y += h3 * (lapFactor + r3 * (1.0 - lapFactor));
    }

    wp1.y += baseline;
    wp2.y += baseline;
    wp3.y += baseline;

    vec3 norm = normalize(cross(wp2 - wp1, wp3 - wp1));

    //re_FragPosition = wp1;
    //re_FragNormal = norm;

    gl_Position = re_VPMatrix * vec4(wp1, 1.0);

    //
    //
    //

    vec3 pos = wp1;

    vec3 blue = 1.5 * vec3(0.13, 0.47, 0.86);
    vec3 teal = 1.2 * vec3(85.0/255.0, 210.0/255.0, 190.0/255.0);

    float ndl = max(dot(norm, -re_LightDirection), 0.0);
    vec3 toEye = normalize(re_Eye - pos);

    float omc = 1.0 - max(0.0, dot(toEye, norm));
    float R = R0 + (1.0 - R0)*omc*omc*omc*omc*omc;
    float T = 1.0 - R;

    float AA = 0.25;
    float SA = 2.7;
    //float DA = 1.0 - SA - AA;
    float DA = 0.65;

    vec3 ambientColor  = AA * (R * blue + T * teal);
    vec3 specularColor = SA * (R * blue + T * teal);
    vec3 diffuseColor  = DA * (R * blue + T * teal);

    vec3 color = vec3(0.0, 0.0, 0.0);
    color += 1.0 * ambientColor;
    color += ndl * diffuseColor;
    //color += ss * specularColor;

    //vec4 color = vec4(133.0/255.0, 219.0/255.0, 215.0/255.0, 0.25); // 0.4

    re_FragColor = vec4(color.rgb, R);
}
