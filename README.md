# GeoSeis-Web-View

## Installation

Download nodejs from https://nodejs.org.

Clone repository to the local folder
```
git clone https://github.com/take5v/GeoSeis-Web-View.git
cd GeoSeis-Web-View
```
Install native c++  module scs3parser first
```
cd native_modules/scs3reader_module_addon
npm install
```
Install all
```
cd ../..
npm install
```

## Running

Simply execute

```
node bin\www
```

or if you could use nodemon or forever for development

```
npm install -g nodemon
nodemon bin\www
```

```
npm install -g forever
forever -w bin\www
```
