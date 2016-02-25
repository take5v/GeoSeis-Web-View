if (typeof SEISREN === 'undefined') {
	SEISREN = {};
}

var SEISREN = (function (SEISREN) {
	"use strict";

	if ( ! Detector.webgl ) Detector.addGetWebGLMessage();

	var container, stats;
	var camera, scene, renderer;
	var seisDisplayableModel;

	// {x, y}
	var onMouseDownPosition = {};
	var onMouseDownCameraPosition = {};
	var isMouseDown = false;

	var effectController = {
		zoomFactor: 1,
		stepX: 10,
		stepY: 20,
		zoomT: 0.006
	};

	/*
	Local functions
	 */
	function initGUI() {
		var gui = new dat.GUI();

		var f1 = gui.addFolder('Camera');
		f1.add(effectController, "zoomFactor", 0.001, 20).listen();
		var f2 = gui.addFolder('Seismo params');
		f2.add(effectController, "stepX", 1, 500).listen();
		f2.add(effectController, "stepY", 1, 500).listen();
		f2.add(effectController, "zoomT", 0.001, 0.01).listen();
		f2.open();
	}

	function initStats() {
		stats = new Stats();
		stats.domElement.style.position = 'absolute';
		stats.domElement.style.top = '0px';
		container.appendChild( stats.domElement );
	}

	function onWindowResize() {
		renderer.setSize( window.innerWidth, window.innerHeight );
	}

	function onWindowMouseDown( event ) {
		event.preventDefault();

		isMouseDown = true;

		onMouseDownPosition.x = event.clientX;
		onMouseDownPosition.y = event.clientY;
		onMouseDownCameraPosition.x = camera.position.x;
		onMouseDownCameraPosition.y = camera.position.y;
	}

	function onWindowMouseMove( event ) {
		event.preventDefault();

		if (isMouseDown) {
			camera.position.x = onMouseDownCameraPosition.x - (event.clientX - onMouseDownPosition.x);
			camera.position.y = onMouseDownCameraPosition.y + (event.clientY - onMouseDownPosition.y);
			SEISREN.render();
		}
	}

	function onWindowMouseUp( event ) {
		event.preventDefault();

		isMouseDown = false;

		onMouseDownPosition.x = (event.clientX - onMouseDownPosition.x) / camera.zoom;
		onMouseDownPosition.y = (event.clientY - onMouseDownPosition.y) / camera.zoom;
	}

	function onWindowMouseWheel( event ) {
		effectController.zoomFactor = event.wheelDeltaY > 0 ? camera.zoom * 1.1 : camera.zoom / 1.1;
		effectController.zoomFactor = effectController.zoomFactor < 0.001 ? 0.001 : effectController.zoomFactor > 20 ? 20 : effectController.zoomFactor;
		SEISREN.render();
	}

	function generateSCS3(discreteCount, traceCount, minV, maxV) {
		var seisData = new SEISREN.SeisData(discreteCount, traceCount, minV, maxV);

		function getRandomInt(min, max) {
			return Math.floor(Math.random() * (max - min)) + min;
		}

		function generateData() {
			var data = new Array(discreteCount);
			for (var i = 0; i < discreteCount; i++) {
				data[i] = getRandomInt(minV, maxV);
			}
			return data;
		}

		for (var i = 0; i < traceCount; i++) {
			var data = generateData();
			seisData.data = seisData.data.concat(data);
		}
		return seisData;
	}

	function loadData(url, callback) {
		var xhr = new XMLHttpRequest();
		xhr.responseType = "json";
		xhr.open("get", url, true);
		xhr.onreadystatechange = function (oEvent) {
			if (xhr.readyState === 4) {
				if (xhr.status === 200) {
					callback(xhr.response);
				}
			}
		};
		xhr.send();
	}

	/*
	API
	 */
	SEISREN.init = function() {
		initGUI();

		container = document.getElementById( 'container' );

		var url = '/data';
		loadData(url, function(seisData) {
			scene = new THREE.Scene();

			camera = new THREE.OrthographicCamera( window.innerWidth / - 2, window.innerWidth / 2, window.innerHeight / 2, window.innerHeight / - 2, 0.1, 1000 );
			camera.position.z = 1;


			seisDisplayableModel = new SEISREN.SeisDisplayableModel(seisData);
			seisDisplayableModel.init(seisData, effectController);
			seisDisplayableModel.addToScene(scene);

			renderer = new THREE.WebGLRenderer({ antialiasing: true });
			renderer.setClearColor(0xFFFFFF, 1);
			renderer.setSize(window.innerWidth, window.innerHeight);
			document.body.appendChild(renderer.domElement);
			renderer.render(scene, camera);

			initStats();

			window.addEventListener( 'resize', onWindowResize );

			window.addEventListener( 'mousemove', onWindowMouseMove, false );
			window.addEventListener( 'mousedown', onWindowMouseDown, false );
			window.addEventListener( 'mouseup', onWindowMouseUp, false );

			window.addEventListener( 'mousewheel', onWindowMouseWheel, false );

			SEISREN.animate();
		});
		//var seisData = generateSCS3(100, 100, -14046, 15427);
	};

	SEISREN.render = function() {
		renderer.render(scene, camera);
	};

	SEISREN.animate = function() {
		requestAnimationFrame( SEISREN.animate );

		seisDisplayableModel.update();
		camera.zoom = effectController.zoomFactor;
		camera.updateProjectionMatrix();
		SEISREN.render();
		stats.update();
	};


	// *
	//  * [SeisTrace description]
	//  * @param {[json]} header [Parsed file header]
	//  * @param {[Int16Array]} data   [Parsed Int16Array]
	 
	// SEISREN.SeisTrace = function(header, data) {
	// 	this.header = header;
	// 	this.data = data;
	// };

	/**
	 * [SeisData description]
	 */
	SEISREN.SeisData = function (discrCount, traceCount, min, max) {
		this.minV = min;
		this.maxV = max;
		this.discreteCount = discrCount;
		this.traceCount = traceCount;
		// data is array of values (discrCount * traceCount) size
		this.data = [];
	};

	SEISREN.SeisData.prototype = {

		constructor: SEISREN.SeisData,

	};

	/**
	 * [SeisDisplayableModel description]
	 */
	SEISREN.SeisDisplayableModel = function () {
		this.lines = [];
		this.seisData = null;
		this.vertices = [];

		this.effectController = null;
	};

	SEISREN.SeisDisplayableModel.prototype = {

		constructor: SEISREN.SeisDisplayableModel,

		init: function(seisData, effectController) {
			this.seisData = seisData;
			this.effectController = effectController;

			var material = new THREE.RawShaderMaterial({
				vertexShader: document.getElementById( 'shader-vs' ).textContent,
				fragmentShader: document.getElementById( 'shader-fs' ).textContent
			});
			
			// var material = new THREE.MeshBasicMaterial({ 
			// 	color:0xFFFFFF
			// }); 


			for (var i = 0; i < this.seisData.traceCount; i++) {
				var geometry = new THREE.BufferGeometry();
				this.vertices[i] = new Float32Array(this.seisData.discreteCount * 3);
				geometry.addAttribute('position', new THREE.BufferAttribute(this.vertices[i], 3));
				this.lines[i] = new THREE.Line(geometry, material);
				//this.lines[i] = new THREE.Mesh(geometry, material);
			}

			this.update();
		},

		update: function() {
			var h05 = this.seisData.discreteCount * this.effectController.stepY / 2;
			var w05 = this.seisData.traceCount * this.effectController.stepX / 2;

			for (var j = 0; j < this.seisData.traceCount; j++) {
				for ( var i = 0; i < this.seisData.discreteCount; i++ )
				{
					var x = j * this.effectController.stepX - (this.seisData.data[this.seisData.discreteCount*j+i] * this.effectController.zoomT) - w05;
					var y = h05 - i * this.effectController.stepY;
					this.vertices[j][ i*3 + 0 ] = x;
					this.vertices[j][ i*3 + 1 ] = y;
					this.vertices[j][ i*3 + 2 ] = 1;
				}
				this.lines[j].geometry.attributes.position.needsUpdate = true;
			}
		},

		addToScene: function(scene) {
			for (var i = 0; i < this.lines.length; i++) {
				scene.add(this.lines[i]);
			}
		}
	};

	return SEISREN;
})(SEISREN);