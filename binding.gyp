{
  "targets": [{
    "target_name": "dope",
    "sources":[
      "./src/addon.c",
      "./src/addon_node.c"
    ],
    "cflags!": ["-g","-fno-exceptions"],
    "cflags_cc!": ["-g","-fno-exceptions"],
    "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
    "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
  }]
}
