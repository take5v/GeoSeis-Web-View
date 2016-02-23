uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

uniform float xPosUniform;
varying float x;

attribute vec2 position;

void main(void) {
	gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 0.0, 1.0 );
	gl_Position[2] = 1.0;

	vec4 xPos = projectionMatrix * modelViewMatrix * vec4( xPosUniform, 0.0, 0.0, 1.0 );
	x = xPos[0];
}