if (typeof SCS3 === 'undefined') {
	SCS3 = {};
}

var SCS3 = (function (SCS3) {
	return SCS3;
})(SCS3);


	"use strict";

	var canvas;
	var gl;

	var vertexBuffer;
	var vertexPositionAttribute;

	var nValues = 3000;
	var nSensors = 100;

	function start() {
		canvas = document.getElementById('glcanvas');

		// init gl
		gl = initWebGL();

		if (gl) {
			gl.clearColor(1.0, 1.0, 1.0, 1.0);
			gl.enable(gl.DEPTH_TEST);
			gl.depthFunc(gl.LEQUAL);

			initShaders();

			initBuffers();

			drawScene();
		}
	}

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

		var shaderProgram = gl.createProgram();
		gl.attachShader(shaderProgram, vertexShader);
		gl.attachShader(shaderProgram, fragmentShader);
		gl.linkProgram(shaderProgram);

		if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
			alert("Unable to initialize the shader program.");
		}

		gl.useProgram(shaderProgram);

		vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
		gl.enableVertexAttribArray(vertexPositionAttribute);
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
		vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);

		var vertices = generateGeoVertices();

		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
	}

	function generateGeoVertices() {
		var startX = 5000;
		var dX = 500;
		var startY = 1000;
		var dY = 100;

		var arrX = [];
		var arrY = [];

		for (var i = 0; i < nSensors; i++) {
			var currX = startX + i*dX;
			for (var j = 0; j < nValues; j++) {
				var currY = startY + j*dY;
				arrX.push(currX  + Math.sin(currY) * dX / 3);
				arrY.push(currY);
			}
		}

		normalize(arrX);
		normalize(arrY);

		var result = [];

		for (var i = 0; i < nSensors*nValues; i++) {
			result.push(arrX[i], arrY[i]);
		}

		return result;
	}

	function normalize(array) {
		var max = Number.MIN_VALUE;
		var min = Number.MAX_VALUE;

		for (var i = 0; i < array.length; i++) {
			max = array[i] > max ? array[i] : max;
			min = array[i] < min ? array[i] : min;
		}

		for (var i = 0; i < array.length; i++) {
			array[i] = 2 * (array[i] - max) / (max - min) + 1;
		}
	}

	function drawScene() {
		resizeCanvas();

		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		gl.vertexAttribPointer(vertexPositionAttribute, 2, gl.FLOAT, false, 0, 0);


		for (var i = 0; i < nSensors; i++) {
			gl.drawArrays(gl.LINE_STRIP, nValues*i, nValues);
		}
	}

	// window.addEventListener('resize', resizeCanvas);
	window.onresize = drawScene;
