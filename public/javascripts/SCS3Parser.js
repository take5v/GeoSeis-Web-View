if (typeof SEISREN === 'undefined') {
	SEISREN = {};
}

var SEISREN = (function (SEISREN) {
	"use strict";

	/*
		ByteStream
	 */
	var ByteStream = function(byteArrayParser, byteArray) {
		this.byteArrayParser = byteArrayParser;
		this.byteArray = byteArray;
		this.position = 0;
	};

	ByteStream.prototype.seek = function(offset) {
		this.position = offset;
	};

	ByteStream.prototype.readInt32 = function() {
		var result = this.byteArrayParser.readInt32(this.byteArray, this.position);
		this.position += 4;
		return result;
	};

	ByteStream.prototype.readByteStream = function(numBytes) {
		var newArray = new Uint8Array(this.byteArray.buffer, this.position, numBytes);
		this.position += numBytes;
		return newArray;
	};

	// Reads a string of 8-bit characters
	ByteStream.prototype.readFixedString = function(length) {
		var result = this.byteArrayParser.readFixedString(this.byteArray, this.position, length);
		this.position += length;
		return result;
	};

	// Byte Array Parsers
	var littleEndianByteArrayParser = {
		readInt32: function(byteArray, position) {
			var int32 = byteArray[position] |
						(byteArray[position + 1] << 8) |
						(byteArray[position + 2] << 16) |
						(byteArray[position + 3] << 24);
			return int32;
		},

		readFixedString: function(byteArray, position, length) {
			var result = "";
			for (var i = 0; i < length; i++) {
				var byte = byteArray[position + i];
				result += String.fromCharCode(byte);
			}
			return result;
		}
	};

	var bigEndianByteArrayParser = {
		readInt32: function(byteArray, position) {
			var int32 = (byteArray[position] << 24) |
						(byteArray[position + 1] << 16) |
						(byteArray[position + 2] << 8) |
						byteArray[position + 3];
			return int32;
		},

		readFixedString: function(byteArray, position, length) {
			var result = "";
			for (var i = length - 1; i >= 0; i--) {
				var byte = byteArray[position + i];
				result += String.fromCharCode(byte);
			}
			return result;
		}
	};

	// parser
	SEISREN.SCS3Parser = function() {

	};


	SEISREN.SCS3Parser.prototype = {
		loadAndParse: function(url) {
			var that = this;
			var xhr = new XMLHttpRequest();
			xhr.responseType = "arraybuffer";
			xhr.open("get", url, true);
			xhr.onreadystatechange = function (oEvent) {
				if (xhr.readyState === 4) {
					if (xhr.status === 200) {
						var arrayBuffer = xhr.response;
						var byteArray = new Uint8Array(arrayBuffer);

						that.parse(byteArray);
					}
				}
			};
			xhr.send();
		},

		parse: function(byteArray) {
			var parserUtils = {
				parseNextHeader: function(stream) {
					// Header for trace
					var TraceHeader = function(stream) {
						this.mg_1 = stream.readInt32();
						this.trace = stream.readInt32();
						this.mg_2 = stream.readInt32();
						this.actual = stream.readInt32();
						this.sp = stream.readInt32();
						this.xsp = stream.readInt32();
						this.xdp = stream.readInt32();
						this.xop = stream.readInt32();
						this.l = stream.readInt32();
						this.form = stream.readFixedString(4);
						this.tbeg = stream.readInt32();
						this.tend = stream.readInt32();
						this.tbp = stream.readInt32();
						this.tep = stream.readInt32();
						this.diskr = stream.readInt32();
						this.p = stream.readInt32();
						this.mod = stream.readInt32();
						this.ver = stream.readInt32();
						this.y = stream.readInt32();
						this.xy = stream.readInt32();
					};
					return new TraceHeader(stream);
				},

				skipNextHeader: function(stream) {
					var bytes = 20;
					stream.seek(stream.position + bytes);
					return bytes;
				},

				getFormat: function(header, nDiscret) {
					var iType;
					var step;
					if (header.form[0] == 'i' || header.form[0] == 'I' || header.form[3] == 201)
					{
						if (header.form[1] == '2' || header.form[2] == 242)
						{
							iType = 1;
							step = nDiscret*2;

						}
						else
						if (header.form[1] == '4' || header.form[2] == 244)
						{
							iType = 2;
							step = nDiscret*4;

						}
						else
						if (header.form[1] == '8')
						{
							iType = 3;
							step = nDiscret*8;
						}
						else
						{
							step = nDiscret*2;
							iType = 1;
						}
					}
					else
					{
						if (header.form[0] == 'r' || header.form[0] == 'R' || header.form[3] == 217)
						{
							if (header.form[1] == '8')
							{
								step = nDiscret*8;
								iType = 5;
							}
							else
							{
								step = nDiscret*4;
								iType = 4;
							}
						}
						else
						{
							step = nDiscret*2;
							iType = 1;
						}
					}

					return {iType: iType, step: step};
				},

				getTraceData: function(byteArray, offset, iType, nDiscret) {
					var data;
					var result = new Float32Array(nDiscret);
					switch(iType){
						case 0:
							data = new Int8Array(byteArray, offset, nDiscret);
							break;
						case 1:
							data = new Int16Array(byteArray, offset, nDiscret);
							for (var i = 0; i < nDiscret; i++) {
						// 		(byteArray[position] << 24) |
						// (byteArray[position + 1] << 16) |
						// (byteArray[position + 2] << 8) |
						// byteArray[position + 3];
								result[i] = (byteArray[2*i+offset] << 8);
							}
							break;
					}

					return result;
				},

				getMinMax: function(array) {
					var min = Number.POSITIVE_INFINITY;
					var max = Number.NEGATIVE_INFINITY;
					var numPixels = array.length;
					for (var i = 0; i < numPixels; i++) {
						var curr = array[i];
						min = Math.min(min, curr);
						max = Math.max(max, curr);
					}

					return {
						min: min,
						max: max
					};

				}
			};
			
			var stream = new ByteStream(bigEndianByteArrayParser, byteArray);

			stream.seek(12);
			var actual = stream.readInt32();
			console.log(actual);

			if (actual !== 0 && actual != 1) {
				console.log('little endian');
				stream = new ByteStream(littleEndianByteArrayParser, byteArray);
				stream.seek(12);
				actual = stream.readInt32();
				if (actual !== 0 && actual != 1) {
					throw "SEISREN.SCS3Parser.parseSCS3: problem with parsing SCS3 file";
				}
			}

			stream.seek(0);

			var header = parserUtils.parseNextHeader(stream);
			console.log(header);

			var discreteCount = (header.tend - header.tbeg) / header.diskr;


			var format = parserUtils.getFormat(header, discreteCount);

			console.log(format);

			console.log(byteArray.byteLength);

			var nTrace = byteArray.byteLength / (80 + format.step);

			var traces = [];

			for (var i = 0; i < 1; i++) {
				var traceData = parserUtils.getTraceData(byteArray, 80, format.iType, discreteCount);
				console.log(parserUtils.getMinMax(traceData));
				console.log(traceData);
			}

















			// var numBytes = 123;
			// var data = stream.readByteStream(numBytes);

			// var trace = {
			// 	header: header,
			// 	data: data
			// };

			// var scs3 = {
			// 	header: {
			// 		width: 800,
			// 		height: 600,
			// 		minValue: -1000,
			// 		maxValue: 1000,
			// 		traceCount: 5000,
			// 		discreteCount: 3000
			// 	},
			// 	traces: [trace]
			// }

			return;
		},
		data: function() {
			return this.data;
		}
	};

	

	// Header for whole scs3 file
	SEISREN.SCS3Parser.Header = function(stream) {
		// stream.seek(0);



		// this.width:
		// this.height:
		// this.minValue: -
		// this.maxValue:
		// this.traceCount:
		// this.discreteCount:
	};

	SEISREN.SCS3Parser.getPixelFormat = function(format) {
		var result;
		if (format[0] == 'i' || format[0] == 'I' || format[3] == 201) {
			if (format[1] == '2' || format[2] == 242) {
				result = 1;
			} else if (format[1] == '4' || format[2] == 244) {
				result = 2;
			} else if (format[1] == '8') {
				result = 3;
			} else {
				result = 1;
			}
		} else if (format[0] == 'r' || format[0] == 'R' || format[3] == 217) {
			if (format[1] == '8') {
				result = 5;
			} else {
				result = 4;
			}
		} else {
			result = 1;
		}
	};

	SEISREN.SCS3Parser.getBytesPerPixel = function() {
		var pixelFormat;
		if (pixelFormat == 1) {
			return 1;
		} else if (pixelFormat == 2 || pixelFormat == 4) {
			return 2;
		} else if (pixelFormat == 3 || pixelFormat == 5) {
			return 4;
		}
		throw "unknown pixel format";
	};

	SEISREN.SCS3Parser.getMinMax = function(storedPixelData) {
		var min = Number.POSITIVE_INFINITY;
		var max = Number.NEGATIVE_INFINITY;
		var numPixels = storedPixelData.length;
		for (var i = 0; i < numPixels; i++) {
			var curr = storedPixelData[i];
			min = Math.min(min, curr);
			max = Math.max(max, curr);
		}

		return {
			min: min,
			max: max
		};
	};

	return SEISREN;
})(SEISREN);

function parse(url) {
	var xhr = new XMLHttpRequest();
	xhr.responseType = "arraybuffer";
	xhr.open("get", url, true);
	xhr.onreadystatechange = function (oEvent) {
		if (xhr.readyState === 4) {
			if (xhr.status === 200) {
				var arrayBuffer = xhr.response;
				var byteArray = new Uint8Array(arrayBuffer);

				SEISREN.SCS3Parser.parseSCS3(byteArray);
			}
		}
	};

	xhr.send();
}