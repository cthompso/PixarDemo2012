#version 120
#extension GL_EXT_geometry_shader4 : enable

// Inputs
//uniform float rotation;
//uniform float size;
uniform float mTime;
uniform vec3 camPos;

varying vec4 coords;
varying float outRand;
varying vec4 outColor;
varying float atten;

//varying vec4 gl_FrontColorIn[];

//varying vec3 v;
float rand(vec2 n)
{
    return 0.5 + 0.5 *
    fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

// Kernel
void main(void)
{
    float size = 10.0;
    float rotation = 1.0 * mTime;
    
    vec3 cubeVerts[8] = vec3[](
        vec3(-1.0,-1.0,-1.0),
        vec3(1.0,-1.0,-1.0),
        vec3(1.0,1.0,-1.0),
        vec3(-1.0,1.0,-1.0),
        vec3(-1.0,-1.0,1.0),
        vec3(1.0,-1.0,1.0),
        vec3(1.0,1.0,1.0),
        vec3(-1.0,1.0,1.0)
    );
    
    int cubeInds [36] = int[](
        2,1,4, 4,2,3, 3,4,8, 8,3,7, 7,8,5, 5,7,6,
        3,7,6, 6,3,2, 2,6,5, 5,2,1, 1,5,8, 8,1,4
      );

    //outColor = cubeColor;
    
    bool opt = true;

    outRand = rand(gl_PositionIn[0].xy);
    outColor = vec4(outRand,1.0,1.0,1.0);
    

    atten = distance(gl_PositionIn[0].xyz,camPos);
    atten = clamp(atten*0.05,0.0,1.0);
    
    if (opt) {
        for (int j = 0; j < 6; j++) {


                        
            int loc = j * 3;
            // Point A
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[0+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();
            // Point B
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[1+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();
            // Point C
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[2+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();

        }
        EndPrimitive();
    }
    if ( opt ) {
        for (int j = 6; j < 12; j++) {
            
            
            int loc = j * 3;
            // Point A
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[0+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();
            // Point B
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[1+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();
            // Point C
            gl_Position = gl_PositionIn[0] + vec4(cubeVerts[cubeInds[2+loc]-1],gl_PositionIn[0].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            
            EmitVertex();

        }
        EndPrimitive();
    }
}

