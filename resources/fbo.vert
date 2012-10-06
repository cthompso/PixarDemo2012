//uniform vec3 LightPosition;
//uniform vec3 SkyColor;
//uniform vec3 GroundColor;
//
//attribute float Accessibility;
//
//varying vec3  DiffuseColor;
//
//
//varying float LightInt;
//varying vec3 mNorm;
//uniform vec3 sPos;
//uniform float area;
//uniform vec3 Direction;
uniform float mTime;
//	
//void main()
//{		
//    vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
//    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
//    vec3 lightVec   = normalize(LightPosition - ecPosition);
//    float costheta  = dot(tnorm, lightVec);
//    float a         = 0.5 + 0.5 * costheta;
//
//    DiffuseColor = mix(GroundColor, SkyColor, a);// * Accessibility;
//
//	vec3 pos = vec3(gl_ModelViewMatrix * gl_Vertex);
//	float close = length(sPos - vec3(0.0,0.0,0.0)) / (area * 0.3);
//
//	float angle = 1.0 - dot(gl_Normal, normalize(sPos));
//	angle = mix(1.0,angle,close);
//	LightInt = clamp(angle,0.0,1.0);
//	mNorm = normalize(gl_Normal);
////	gl_TexCoord[0] =  gl_MultiTexCoord0;
//
//	vec3 stretch = (sPos - Direction) * vec3(1,1,0);
//	float amt = dot(normalize(gl_Normal),normalize(stretch)) * length(stretch) * ((close) * 1.5);
//	//LightInt = amt;
//	//amt *= 50.0;
//	//newVertexPos = vec4(gl_Normal * df * 100.0, 0.0) + gl_Vertex;
////	gl_Position = vec4(stretch, 0) + ftransform() ;//* stretch;
//	vec4 newPos = vec4(normalize(stretch) * amt, 0.0) + gl_Vertex;
//	gl_Position = gl_ModelViewProjectionMatrix * newPos;
//}

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}

