#version 110

uniform sampler2D tex0;
uniform float mtime;
uniform float mfade;
uniform float ease;

float weights[21];

void main()
{
	weights[0] = 0.0091679276560113852;
	weights[1] = 0.014053461291849008;
	weights[2] = 0.20595286319257878;
	weights[3] = 0.028855245532226279;
	weights[4] = 0.038650411513543079;
	weights[5] = 0.049494378859311142;
	weights[6] = 0.000594058578763078;
	weights[7] = 0.000921288047096992;
	weights[8] = 0.009358891804948081;
	weights[9] = 0.024895951965930902;
	weights[10] = 0.006826196862124602;
	weights[11] = 0.084895951965930902;
	weights[12] = 0.079358891804948081;
	weights[13] = 0.000921288047096992;
	weights[14] = 0.060594058578763092;
	weights[15] = 0.049494378859311121;
	weights[16] = 0.0386504115135431;
	weights[17] = 0.028855245532226279;
	weights[18] = 0.020595286319257885;
	weights[19] = 0.014053461291849008;
	weights[20] = 0.0916792765601138;
    
    vec2 coords = gl_TexCoord[0].st;

    coords.s += sin(coords.t*10000.0+mtime)*ease;
    if (mtime > 45.0) {
        coords.s += sin(coords.t*10000.0+mtime)*weights[int(mod(mtime,20.0))];
    }

    vec4 tex = texture2D(tex0, coords);
	gl_FragColor.rgb = tex.rgb;
	gl_FragColor.a = tex.r * mfade;
}