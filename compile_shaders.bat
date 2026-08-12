glslc -fshader-stage=vertex data/shaders/vertex.glsl -o data/shaders/vertex.spv
glslc -fshader-stage=fragment data/shaders/fragment.glsl -o data/shaders/fragment.spv

dxc -spirv -T vs_6_0 -E main data/shaders/batcher_vertex.hlsl -Fo data/shaders/batcher_vertex.spv
dxc -spirv -T ps_6_0 -E main data/shaders/batcher_fragment.hlsl -Fo data/shaders/batcher_fragment.spv
