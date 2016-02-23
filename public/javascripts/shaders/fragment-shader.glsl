precision highp float;

varying float x;

void main(void) {
	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	//gl_FragColor = (x + vec4(1.0, 0.0, 0.0, 1.0)) * 0.5;

//	vec4 color = vec4((1.0 + x) * 0.5, 0.0, 0.0, 1.0) * 0.5;
	//gl_FragColor = vec4((1.0 + x) * 0.5, 0.0, 0.0, 1.0) * 0.5;
	//float width = 1412.0;
	//if ((2.0 * float(gl_FragCoord.x) / width) - 1.0  > x) {
		//gl_FragColor[3] = 0.0;
	//}

	//gl_FragColor = color;
}