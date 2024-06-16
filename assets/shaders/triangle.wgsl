struct VertexIn {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
}

struct VertexOut {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@group(0) @binding(0)
var<uniform> model: mat4x4<f32>;
@group(0) @binding(1)
var<uniform> projection: mat4x4<f32>;
@group(0) @binding(2)
var<uniform> view: mat4x4<f32>;

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
    var out: VertexOut;
    out.position = projection * view * model * vec4<f32>(in.position, 1.0f);
    // out.position = vec4<f32>(in.position, 1.0f);
    out.uv = in.uv;
    return out;
}

@group(1) @binding(0)
var sampler2d: sampler;
@group(1) @binding(1)
var texture2d: texture_2d<f32>;

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4<f32> {
    return textureSample(texture2d, sampler2d, in.uv);
    // return vec4<f32>(1.0f, 0.5f, 0.3f, 1.0f);
}