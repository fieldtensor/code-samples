uniform vec3 re_LightDirection;
uniform mat4 re_VPMatrix;
uniform mat4 re_WorldMatrix;

attribute vec4 re_Position;

const float nudge = 0.002;
uniform float deep;

void
main()
{
    if( re_Position.w == 0.0 )
    {
        /*
        The default method is to project shadow volumes out to infinity. This
        is commented out in favor of the optimized method further below.
        */

        // gl_Position = re_VPMatrix * vec4(re_LightDirection.xyz, 0.0);

        /*
        The optimized method is to project the shadow volumes down to the
        ocean floor, and no further. This greatly reduces the fill rate. Based
        on some quick performance tests it takes the GPU 4.2 ms to fill the
        unoptimized case and 0.3 ms to fill the optimized case. This is not a
        typing mistake or a measuring glitch. The benefit is indeed an entire
        order of magnitude. At 60 FPS the unoptimized case requires 25% of the
        frame time just for shadowing, while the optimized case requires just
        2% of the frame time. The benefit of this optimization is tremendous.
        */

        vec3 rayOrigin = (re_WorldMatrix * vec4(re_Position.xyz, 1.0)).xyz;
        float d = ((deep-nudge) - rayOrigin.y) / re_LightDirection.y;

        gl_Position = re_VPMatrix * vec4(
            rayOrigin + d * re_LightDirection, 1.0);
    }
    else
    {
        gl_Position = re_VPMatrix * re_WorldMatrix * re_Position;
    }
}
