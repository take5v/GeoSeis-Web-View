$(document).ready(function() {
	//init();

	//load scripts
	$.when($.ajax('javascripts/shaders/vertex-shader.glsl'), $.ajax('javascripts/shaders/fragment-shader.glsl'))
		.done(function(vs, fs, st1, st2) {
			$('#shader-vs').html(vs[0]);
			$('#shader-fs').html(fs[0]);

			// start rendering
			//SCS3.init();
			//SCS3.animate();
			SEISREN.init();
		});
});