{
  "targets": [
    {
      "target_name": "heatshrink",
      "sources": ["./src/addon.c", "./src/addon_node.c"]
    },
    {
      "target_name": "types",
      "type": "none",
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release",
          "files": ["./src/heatshrink.d.ts"]
        }
      ],
      "cflags!": ["-g", "-fno-exceptions"],
      "cflags_cc!": ["-g", "-fno-exceptions"],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"]
    }
  ]
}
