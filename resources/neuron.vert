varying vec4 ecPosition;
varying vec3 ecPosition3;
varying vec3 normal;
varying vec4 ambientGlobal;
varying float dist;

void main()
{
	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;
    
    
    vec4 camera_pos = gl_ModelViewMatrixInverse[3];
    dist = 1.0 - (distance(camera_pos.xyz, gl_Vertex.xyz) /20.0);
    
	normal			= gl_NormalMatrix * gl_Normal;
    
    
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;

	
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
}