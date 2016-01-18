$(document).ready(function() {
	//init();

	// parse('data/D012190R(формат PC IBM).SEC');

	//parse('data/D190794.SCS3');

	// parse('data/D012190R(формат ЕС ЭВМ).SEC');


	//load scripts
	$.when($.ajax('javascripts/shaders/vertex-shader.glsl'), $.ajax('javascripts/shaders/fragment-shader.glsl'))
		.done(function(vs, fs, st1, st2) {
			$('#shader-vs').html(vs[0]);
			$('#shader-fs').html(fs[0]);

			// start rendering
			SEISREN.init();
			SEISREN.animate();
		});
});