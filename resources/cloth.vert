varying vec4 ecPosition;
varying vec3 ecPosition3;
varying vec3 normal;
varying vec4 ambientGlobal;
uniform float mTime;

float rand2X(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	ecPosition		= gl_ModelViewMatrix * gl_Vertex;
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;
    
	normal			= gl_NormalMatrix * gl_Normal;
    
    
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex ;
    //vec4 HPos1 = gl_ModelViewProjectionMatrix * gl_Vertex;
    ///vec4 Po = vec4(gl_Vertex.xyz, 1.0);
	//vec4 HPos2 = vec4(gl_TexCoord.x*2.0 - 1.0, -(gl_TexCoord.y*2.0 - 1.0) , 0.0,1.0);
	//gl_Position= lerp(HPos1, HPos2, 0.5);
	gl_TexCoord[0] =  gl_MultiTexCoord0 ;
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
 //   vec2 inUV =  vec2(rand2X(gl_MultiTexCoord0.xy),rand2X(gl_MultiTexCoord0.xy)) ;
//	vec4 HPos2 = vec4(inUV.x*2.0 - 1.0, -(inUV.y*2.0 - 1.0) , 0.0,1.0);
	//gl_Position = mix(gl_Position, HPos2, (sin(50.0 *mTime)*0.5 + 0.5) * 0.05);

}