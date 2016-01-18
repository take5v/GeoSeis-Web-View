
if (typeof SCS3Parser === 'undefined') {
	SCS3Parser = {};
}

var SCS3Parser = (function (SCS3Parser) {
	"use strict";

	// ByteStream
	SCS3Parser.ByteStream = function(byteArrayParser, byteArray) {
		this.byteArrayParser = byteArrayParser;
		this.byteArray = byteArray;
		this.position = 0;
	};

	SCS3Parser.ByteStream.prototype.seek = function(offset) {
		this.position = offset;
	};

	SCS3Parser.ByteStream.prototype.readInt32 = function() {
		var result = this.byteArrayParser.readInt32(this.byteArray, this.position);
		this.position += 4;
		return result;
	};

	SCS3Parser.ByteStream.prototype.readByteStream = function(numBytes) {
		var newArray = new Uint8Array(this.byteArray.buffer, this.position, numBytes);
		this.position += numBytes;
		return newArray;
	};

	// Reads a string of 8-bit characters
	SCS3Parser.ByteStream.prototype.readFixedString = function(length) {
		var result = "";
		for (var i = 0; i < length; i++) {
			var byte = this.byteArray[this.position + i];
			result += String.fromCharCode(byte);
		}
		this.position += length;
		return result;
	};

	// Byte Array Parsers
	SCS3Parser.littleEndianByteArrayParser = {
		readInt32: function(byteArray, position) {
			var int32 = byteArray[position] |
						(byteArray[position + 1] << 8) |
						(byteArray[position + 2] << 16) |
						(byteArray[position + 3] << 24);
			return int32;
		}
	};

	SCS3Parser.bigEndianByteArrayParser = {
		readInt32: function(byteArray, position) {
			var int32 = (byteArray[position] << 24) |
						(byteArray[position + 1] << 16) |
						(byteArray[position + 2] << 8) |
						byteArray[position + 3];
			return int32;
		}
	};

	// parse function
	SCS3Parser.parseSCS3 = function(byteArray) {
		var stream = new SCS3Parser.ByteStream(SCS3Parser.littleEndianByteArrayParser, byteArray);

		stream.seek(12);
		var actual = stream.readInt32();

		if (actual !== 0 && actual != 1) {
			console.log('big endian');
			stream = new SCS3Parser.ByteStream(SCS3Parser.bigEndianByteArrayParser, byteArray);
			stream.seek(12);
			actual = stream.readInt32();
			if (actual !== 0 && actual != 1) {
				throw "SCS3Parser.parseSCS3: problem with parsing SCS3 file";
			}
		}

		stream.seek(0);

		var header = SCS3Parser.parseNextHeader(stream);
		console.log(header);

		// var discreteCount = (header.tend - header.tbeg) / header.diskr;

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
	};

	SCS3Parser.parseNextHeader = function(stream) {
		return new SCS3Parser.TraceHeader(stream);
	};

	// Header for every trace
	SCS3Parser.TraceHeader = function(stream) {
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

	// Header for whole scs3 file
	SCS3Parser.Header = function(stream) {
		// stream.seek(0);



		// this.width:
		// this.height:
		// this.minValue: -
		// this.maxValue:
		// this.traceCount:
		// this.discreteCount:
	};

	SCS3Parser.getPixelFormat = function(format) {
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

	SCS3Parser.getBytesPerPixel = function() {
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

	SCS3Parser.getMinMax = function(storedPixelData) {
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

	return SCS3Parser;
})(SCS3Parser);

function parse(url) {
	var xhr = new XMLHttpRequest();
	xhr.responseType = "arraybuffer";
	xhr.open("get", url, true);
	xhr.onreadystatechange = function (oEvent) {
		if (xhr.readyState === 4) {
			if (xhr.status === 200) {
				var arrayBuffer = xhr.response;
				var byteArray = new Uint8Array(arrayBuffer);

				SCS3Parser.parseSCS3(byteArray);
			}
		}
	};

	xhr.send();
}