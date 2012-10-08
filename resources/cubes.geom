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
	// Iterate through vertices
	for (int i = 0; i < gl_VerticesIn; i++)
	{
        
		// Point A
		gl_Position = gl_PositionIn[i] + 2.0;
		EmitVertex();
        
		// Point B
		gl_Position.x += cos(gl_Position.y * rotation) * size;
		gl_Position.y += sin(gl_Position.y * rotation) * size;
		EmitVertex();
        
		// Point C
		gl_Position.x -= cos(gl_Position.y * rotation) * size;
		gl_Position.y -= sin(gl_Position.y * rotation) * size;
		EmitVertex();
        
	}
    
	// Close shape (implied -- this line is not necessary
	// if you are only drawing one shape)
	EndPrimitive();
    
}

