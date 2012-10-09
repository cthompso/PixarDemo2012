

//uniform vec3 LightPosition;
//uniform vec3 SkyColor;
// Inputs
uniform float amp;
uniform float phase;
uniform float rotation;
uniform float scale;
uniform float size;
uniform float speed;
uniform float width;
uniform float mTime;

varying vec4 coords;


// Kernel
void main(void)
{
    
    //cubeColor = vec4(sin(gl_Vertex.x)+1.0,cos(gl_Vertex.y)+1.0,sin(gl_Vertex.z)+1.0,1.0);

    coords = gl_MultiTexCoord0;
    gl_TexCoord[0] = gl_MultiTexCoord0;
	// Position vertex from time
	vec4 vertex = gl_Vertex;
//    vertex.x = sin(mTime) * 2.0;
	float wave = (sin((phase * speed) + vertex.x * width)) * (amp * scale);
//	gl_Position = gl_ModelViewProjectionMatrix * vec4(scale * vertex.x, scale * vertex.y + wave, scale * vertex.z - wave, 1.0);
//    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_Position = vertex;


}

