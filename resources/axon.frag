uniform int NumEnabledLights;
varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ambientGlobal;

varying float dist;

uniform float energy;



void main()
{
    float cVal = clamp(dist+0.1, 0.0, 1.0);
   // cVal += energy*50.0;
    cVal = clamp(cVal, 0.0, 1.0);
	gl_FragColor.rgb = vec3(cVal,cVal,cVal);
	gl_FragColor.a = 1.0;
}
