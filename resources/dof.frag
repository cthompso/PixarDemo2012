#version 110

uniform sampler2D	tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

uniform float blurView;

void main()
{ 

    
    
    vec4 origTex = texture2D( tex0, gl_TexCoord[0].st);
    vec4 blurTex = texture2D( tex1, gl_TexCoord[0].st);
    vec4 depthTex = texture2D( tex2, gl_TexCoord[0].st);
    
    
    //float t = 2.0 * (depthTex.r - 0.5);

    float dVal = depthTex.r;
   // float t =clamp( (dVal -0.85)* 10.0, 0.0, 1.0);//abs(dVal *2.0 - 1.0) ;
     float t =clamp( (dVal -0.87)* 10.0, 0.0, 1.0);//abs(dVal *2.0 - 1.0) ;
    t *= t*t ;
    
    float xval =  abs(gl_TexCoord[0].s- 0.5);
    float yval =  abs(gl_TexCoord[0].t- 0.5);
    float dist =  blurView*sqrt(xval*xval + yval * yval);
   // dist = 0.0;
    float finalBlur = clamp(t + dist, 0.0, 1.0);
	//gl_FragColor.rgb = vec3(blurView,0,0);
gl_FragColor.rgb = mix(origTex.rgb, blurTex.rgb, finalBlur);
    
    gl_FragColor.a = 1.0 ;
}