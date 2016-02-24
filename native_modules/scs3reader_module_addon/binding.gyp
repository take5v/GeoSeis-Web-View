{
  "targets": [
    {
      "target_name": "scs3reader_addon",
      "sources": [
	      "scs3reader_addon.cc",
	      "seismogram.cc",
	      "seismogramimpl.cc"
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
            'OTHER_LDFLAGS': ['-stdlib=libc++'],
            'MACOSX_DEPLOYMENT_TARGET': '10.10'
          }
        }]
      ]
    }
  ]
}
