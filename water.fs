precision highp float;

varying vec3 re_FragPosition;
varying vec3 re_FragNormal;
varying vec4 re_FragColor;

uniform vec3 re_Eye;
uniform vec3 re_LightDirection;

uniform bool colorOn;

uniform vec3 forward;
uniform vec3 up;
uniform vec3 side;
uniform float near;
uniform float aspect;
uniform float fov;
uniform float width;
uniform float height;

uniform float waterScale;

uniform sampler2D refTex;
uniform sampler2D causticMask;

const float R0 = 0.625;

// 1.00029
// 1.33

//const float R0 = 0.02;
//const float R0 = 0.2;

void
main(void)
{
    if( ! colorOn )
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }


    /*
    float spec = 128.0;

    vec3 ref = reflect(re_LightDirection, norm);
    float edr = max(dot(toEye, ref), 0.0);
    float ss = pow(edr, spec);
    */

    /*
    float mtvu = (7.0 + re_FragPosition.x) / 14.0;
    float mtvv = (7.0 + re_FragPosition.z) / 14.0;
    vec4 mtv = texture2D(causticMask, vec2(mtvu, mtvv));
    */

    /*
    vec3 blue = 1.5 * vec3(0.13, 0.47, 0.86);
    vec3 teal = 1.2 * vec3(85.0/255.0, 210.0/255.0, 190.0/255.0);

    vec3 norm = re_FragNormal;
    float ndl = max(dot(norm, -re_LightDirection), 0.0);
    vec3 toEye = normalize(re_Eye - re_FragPosition);

    float omc = 1.0 - dot(toEye, norm);
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

    gl_FragColor = vec4(color.rgb, R);
    */

    gl_FragColor = re_FragColor;
}
