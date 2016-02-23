if (typeof SCS3 === 'undefined') {
	SCS3 = {};
}

var SCS3 = (function (SCS3) {
	"use strict";

	var canvas;
	var gl;

	var vertexLineBuffer;
	var vertexPolysBuffer;
	var shaderProgram;

	var nValues = 30;
	var nSensors = 10;

	var mvMatrix = mat4.create();
	var pMatrix = mat4.create();

	var xUniforms;

	SCS3.init = function(){
		canvas = document.getElementById('glcanvas');

		// init gl
		gl = initWebGL();

		if (gl) {
			gl.clearColor(0.0, 0.0, 0.0, 1.0);
			gl.enable(gl.DEPTH_TEST);
			gl.depthFunc(gl.LEQUAL);

			// alpha
			gl.enable(gl.BLEND);
			gl.blendEquation( gl.FUNC_ADD );
			gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);


			initShaders();

			initBuffers();

			initScene();

			drawScene();

			window.addEventListener('resize', resizeCanvas);
		}
	};

	SCS3.animate = function() {
		requestAnimationFrame(SCS3.animate);

		drawScene();
	};

	function initWebGL() {
		gl = null;

		try {
			gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
		}
		catch(e) {}

		if (!gl) {
			alert("Unable to init WebGL.");
			gl = null;
		}

		return gl;
	}

	function resizeCanvas() {
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
	}

	function initShaders() {
		var fragmentShader = getShader(gl, "shader-fs");
		var vertexShader = getShader(gl, "shader-vs");

		shaderProgram = gl.createProgram();
		gl.attachShader(shaderProgram, vertexShader);
		gl.attachShader(shaderProgram, fragmentShader);
		gl.linkProgram(shaderProgram);

		if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
			alert("Unable to initialize the shader program.");
		}

		gl.useProgram(shaderProgram);

		shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "position");
		gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);

	    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "projectionMatrix");
	    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "modelViewMatrix");

	    shaderProgram.xPosUniform = gl.getUniformLocation(shaderProgram, "xPosUniform");
	}

	function getShader(gl, id) {
		var shaderScript, theSource, currentChild, shader;

		shaderScript = document.getElementById(id);

		if (!shaderScript) {
			return null;
		}

		theSource = "";
		currentChild = shaderScript.firstChild;

		while(currentChild) {
			if (currentChild.nodeType == currentChild.TEXT_NODE) {
				theSource += currentChild.textContent;
			}
			currentChild = currentChild.nextSibling;
		}

		if (shaderScript.type == "x-shader/x-fragment") {
			shader = gl.createShader(gl.FRAGMENT_SHADER);
		} else if (shaderScript.type == "x-shader/x-vertex") {
			shader = gl.createShader(gl.VERTEX_SHADER);
		} else {
			return null;
		}

		gl.shaderSource(shader, theSource);
		gl.compileShader(shader);

		if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
			alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
			return null;
		}

		return shader;
	}

	function initBuffers() {
		vertexLineBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexLineBuffer);

		var vertices = generateGeoVertices();
		vertexLineBuffer.vertices = vertices.lines;

		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexLineBuffer.vertices), gl.STATIC_DRAW);

		xUniforms = new Float32Array(nSensors);
		for (var i = 0; i < nSensors; i++) {
			xUniforms[i] = vertexLineBuffer.vertices[i*nValues*2];
		}

		vertexPolysBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexPolysBuffer);
		vertexPolysBuffer.polys = vertices.polys;
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexPolysBuffer.vertices), gl.STATIC_DRAW);
	}

	function generateGeoVertices() {
		var startX = -500;
		var dX = 100;
		var startY = -200;
		var dY = 15;

		var arrX = [];
		var arrY = [];

		var result = {
			lines: [],
			polys: {
				vertices: [],
				lengths: []
			}
		};

		var isNewPoly = true, currPolyIndex = 0;
		var newPoly;
		var isPolyMaking = false;
		var x;
		var isFirstPointChecked;
		var prevP = {};
		var currY;
		for (var i = 0; i < nSensors; i++) {
			var currX = startX + i*dX;
			// result.lines.push(currX, startY);

			isFirstPointChecked = false;

			for (var j = 0; j < nValues; j++) {
				prevP.x = x;
				prevP.y = currY;

				currY = startY + j*dY;
				x = currX + Math.cos(currY) * dX / 3;

				if (x > currX) {
					if (isNewPoly) {
						var y0 = isFirstPointChecked ? [currX, startY] : [currX, intersect(prevP.x, prevP.y, x, currY, currX)];
						result.polys.vertices.push(currX, y0);
						result.polys.lengths[currPolyIndex] = 1;
						isPolyMaking = true;
						isNewPoly = false;
					}
					result.polys.vertices.push(x, currY);
					result.polys.lengths[currPolyIndex] += 1;
				} else if (isPolyMaking) {
					result.polys.vertices.push(currX, intersect(x, currY, prevP.x, prevP.y, currX));
					result.polys.lengths[currPolyIndex++] += 1;
					isPolyMaking = false;
					isNewPoly = true;
				}
				result.lines.push(x, currY);
			}
		}

		return result;
	}

	function intersect(x1, y1, x2, y2, x0) {
		return ((y2 - y1) * x0 + y1 * x2 - x1 * y2) / x2 - x1;
	}


	// function generateGeoVertices() {
	// 	var startX = 5000;
	// 	var dX = 500;
	// 	var startY = 1000;
	// 	var dY = 100;

	// 	var arrX = [];
	// 	var arrY = [];

	// 	for (var i = 0; i < nSensors; i++) {
	// 		var currX = startX + i*dX;
	// 		for (var j = 0; j < nValues; j++) {
	// 			var currY = startY + j*dY;
	// 			arrX.push(currX  + Math.sin(currY) * dX / 3);
	// 			arrY.push(currY);
	// 		}
	// 	}

	// 	normalize(arrX);
	// 	normalize(arrY);

	// 	var result = [];

	// 	for (var i = 0; i < nSensors*nValues; i++) {
	// 		result.push(arrX[i], arrY[i]);
	// 	}

	// 	return result;
	// }

	function normalize(array) {
		var max = Number.MIN_VALUE;
		var min = Number.MAX_VALUE;

		for (var i = 0; i < array.length; i++) {
			max = array[i] > max ? array[i] : max;
			min = array[i] < min ? array[i] : min;
		}

		for (i = 0; i < array.length; i++) {
			array[i] = 2 * (array[i] - max) / (max - min) + 1;
		}
	}

	function initScene() {
		resizeCanvas();
		mat4.ortho(pMatrix, window.innerWidth / - 2, window.innerWidth / 2, window.innerHeight / 2, window.innerHeight / - 2, 0.1, 1000);
	}

	function drawScene() {
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		gl.bindBuffer(gl.ARRAY_BUFFER, vertexLineBuffer);
		gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, 2, gl.FLOAT, false, 0, 0);

		setMatrixUniforms();

		for (var i = 0; i < nSensors; i++) {
			gl.uniform1f(shaderProgram.xPosUniform, xUniforms[i]);
			gl.drawArrays(gl.LINE_STRIP, nValues*i, nValues);
		}
		
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexPolysBuffer);
		var polysIndex = 0;
		for (var j = 0; j < vertexPolysBuffer.polys.lengths.length; j++) {
		 	gl.drawArrays(gl.TRIANGLE_FAN, polysIndex, vertexPolysBuffer.polys.lengths[j]);
		 	polysIndex += vertexPolysBuffer.polys.lengths[j];
		}
	}

	function setMatrixUniforms() {
		gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
		gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
	}

	return SCS3;
})(SCS3);
