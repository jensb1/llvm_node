{
  "targets": [
    {
      "target_name": "llvm_nodejs",
      "sources": [
        "llvm_context.cpp",
        "llvm_module.cpp",
        "llvm_types.cpp",
        "llvm_builder.cpp",
        "llvm_function.cpp",
        "addon.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<!@(llvm-config --includedir)"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "libraries": [
        "<!@(llvm-config --ldflags --libs core orcjit native)"
      ],
      "cflags": [
        "<!@(llvm-config --cflags)"
      ],
      "cflags_cc": [
        "<!@(llvm-config --cxxflags)",
        "-fexceptions",
        "-frtti"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        ["OS=='mac'", {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "GCC_ENABLE_CPP_RTTI": "YES",
            "MACOSX_DEPLOYMENT_TARGET": "10.15",
            "OTHER_CFLAGS": [
              "<!@(llvm-config --cflags)"
            ],
            "OTHER_CPLUSPLUSFLAGS": [
              "<!@(llvm-config --cxxflags)"
            ]
          }
        }]
      ]
    }
  ]
}
