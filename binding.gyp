{
  "targets": [
    {
      "target_name": "heatshrink",
      "sources": ["./src/addon.c", "./src/addon_node.c", "./src/lib/heatshrink_encoder.c", "./src/lib/heatshrink_decoder.c"],
      "cflags!": ["-g", "-fno-exceptions"],
      "cflags_cc!": ["-g", "-fno-exceptions"],
      "include_dirs": ["src", "lib", "src/lib", "<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"]
    },
    {
      "target_name": "types",
      "type": "none",
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release",
          "files": ["./src/heatshrink.d.ts"]
        }
      ]
    }
  ]
}
