uniform int NumEnabledLights;
varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ambientGlobal;
uniform float mTime;

void PointLight(in int i,
in vec3 eye,
in vec3 ecPosition3,
in vec3 normal,
inout vec4 ambient,
inout vec4 diffuse,
inout vec4 specular,
inout float attenuation )
{
    float nDotVP;         // normal . light direction
    float nDotHV;         // normal . light half vector
    float pf;             // power factor
    float d;              // distance from surface to light source
    vec3  VP;             // direction from surface to light position
    vec3  halfVector;     // direction of maximum highlights
    
    // Compute vector from surface to light position
    VP = vec3( gl_LightSource[0].position ) - ecPosition3;
    
    // Compute distance between surface and light position
    d = length( VP );
    
    // Normalize the vector from surface to light position
    VP = normalize( VP );
    
    // Compute attenuation
    attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation +
    gl_LightSource[0].linearAttenuation * d +
    gl_LightSource[0].quadraticAttenuation * d * d );
    
    halfVector = normalize( VP + eye );
	
	nDotVP = max( 0.0, dot( normal, VP ) );
    nDotHV = max( 0.0, dot( normal, halfVector ) );
    
    if (nDotVP == 0.0)
    pf = 0.0;
    else
    pf = pow( nDotVP, gl_FrontMaterial.shininess );
    
    ambient += gl_LightSource[0].ambient * attenuation;
    diffuse += gl_LightSource[0].diffuse * nDotVP * attenuation;
	specular += gl_LightSource[0].specular * pf * attenuation;
}



vec3 irradcoeefs (      vec3 L00, vec3 L1_1, vec3 L10, vec3 L11,
vec3 L2_2, vec3 L2_1, vec3 L20, vec3 L21, vec3 L22,
vec3 n)
{
    //------------------------------------------------------------------
    // These are variables to hold x,y,z and squares and products
    
	float x2 ;
	float  y2 ;
	float z2 ;
	float xy ;
	float  yz ;
	float  xz ;
	float x ;
	float y ;
	float z ;
	vec3 col ;
    //------------------------------------------------------------------
    // We now define the constants and assign values to x,y, and z
	
	const float c1 = 0.429043 ;
	const float c2 = 0.511664 ;
	const float c3 = 0.743125 ;
	const float c4 = 0.886227 ;
	const float c5 = 0.247708 ;
	x = n[0] ; y = n[1] ; z = n[2] ;
    //------------------------------------------------------------------
    // We now compute the squares and products needed
    
	x2 = x*x ; y2 = y*y ; z2 = z*z ;
	xy = x*y ; yz = y*z ; xz = x*z ;
    //------------------------------------------------------------------
    // Finally, we compute equation 13
    
	col = c1*L22*(x2-y2) + c3*L20*z2 + c4*L00 - c5*L20 + 2.0*c1*(L2_2*xy + L21*xz + L2_1*yz)   + 2.0*c2*(L11*x+L1_1*y+L10*z) ;
	//col = L22 * c1 ;
    
	return col ;
    
}




void main()
{
	vec4 color = vec4(0.1, 0.1, 0.3, 1.0);
	vec3 eye = normalize( -ecPosition3 );
	
	// Clear the light intensity accumulators
	vec4 amb  = vec4( 0.0 );
	vec4 diff = vec4( 0.0 );
	vec4 spec = vec4( 0.0 );
	
	float ppDiffuse			= abs( dot( normal, eye ) );
	float ppFresnell		= 1.0 - ppDiffuse;
	
	float atten = 0.0;
	
	for( int i=0; i<3; i++ )
	{
		PointLight( i, eye, ecPosition3, normalize( normal ), amb, diff, spec, atten );
	}
	
	color += amb * 0.2 +
    diff * 1.0;
    
	color += spec * 0.3;
	
	color += atten * 0.1 * ppFresnell * 1.0;
	color += pow( atten, 3.0 ) * 0.1;
    
    
    const vec3 grace00  = vec3( 0.078908,  0.043710,  0.054161 ) ;
	const vec3 grace1_1 = vec3(  0.039499,  0.034989,  0.060488 ) ;
	const vec3 grace10  = vec3( -0.033974, -0.018236, -0.026940 ) ;
	const vec3 grace11  = vec3( -0.029213, -0.005562,  0.000944 ) ;
	const vec3 grace2_2 = vec3( -0.011141, -0.005090, -0.012231 ) ;
	const vec3 grace2_1 = vec3( -0.026240, -0.022401, -0.047479 ) ;
	const vec3 grace20  = vec3( -0.015570, -0.009471, -0.014733 ) ;
	const vec3 grace21  = vec3(  0.056014,  0.021444,  0.013915 ) ;
	const vec3 grace22  = vec3(  0.021205, -0.005432, -0.030374 ) ;
    vec3 col = irradcoeefs(grace00, grace1_1, grace10, grace11, grace2_2, grace2_1, grace20, grace21, grace22,normal)  *8.0;
	
	gl_FragColor.rgb = col.rgb;
    
    float xval =  abs(gl_TexCoord[0].s- 0.5);
    float yval =  abs(gl_TexCoord[0].t- 0.5);
    float dist = 2.0 * sqrt(xval*xval + yval * yval) - 0.2*(sin(0.01 * mTime) * 0.5 + 0.5);
    dist = clamp(dist, 0.0, 1.0);
  //  gl_FragColor.rgb = vec3(1.0 - dist, 0.0, 0.0);
    gl_FragColor.a =  1.0 - dist;
}
