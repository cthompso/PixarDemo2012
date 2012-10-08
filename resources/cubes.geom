#version 120
#extension GL_EXT_geometry_shader4 : enable

// Inputs
//uniform float rotation;
//uniform float size;
uniform float mTime;

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
        1,2,3, 3,4,1, 4,3,7, 7,8,4, 6,5,8, 8,7,6, 5,6,2, 2,1,5, 5,1,4, 4,8,5, 2,6,7, 7,3,2
    );
	// Iterate through vertices
	for (int i = 0; i < gl_VerticesIn; i++)
	{
     
        for (int j = 0; j < 12; j++) {
            
            int loc = j * 3;
            // Point A
            gl_Position = gl_PositionIn[i] + vec4(cubeVerts[cubeInds[0+loc]-1],gl_PositionIn[i].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;            
            EmitVertex();
            // Point B
            gl_Position = gl_PositionIn[i] + vec4(cubeVerts[cubeInds[1+loc]-1],gl_PositionIn[i].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
            EmitVertex();
            // Point C
            gl_Position = gl_PositionIn[i] + vec4(cubeVerts[cubeInds[2+loc]-1],gl_PositionIn[i].w);
            gl_Position = gl_ModelViewProjectionMatrix * gl_Position;            
            EmitVertex();
        
        }
        
	}
    
	// Close shape (implied -- this line is not necessary
	// if you are only drawing one shape)
	EndPrimitive();
    
}

