var reader = require('./build/Release/scs3reader_addon');

// console.log(reader.parse('123')); // 'world'
// console.log(reader);
var parser = new reader.Seismogram();
var seisData = parser.parse('/Users/take5v/Repository/UUIP/nedra/GeoSeis-Web-View/public/data/D190794.SCS3');
