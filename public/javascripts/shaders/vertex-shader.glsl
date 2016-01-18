uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

attribute vec2 position;

void main(void) {
	gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 0.0, 1.0 );
}