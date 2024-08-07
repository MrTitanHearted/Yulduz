@group(0) @binding(0) var texture: texture_storage_2d<rgba8unorm, write>;

@compute
@workgroup_size(16, 16, 1)
fn cs_main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let tex_size = textureDimensions(texture);
    let uv = vec2<f32>(f32(gid.x) / f32(tex_size.x), f32(gid.y) / f32(tex_size.y));

    textureStore(texture, vec2<u32>(gid.xy), vec4<f32>(uv, 0.0, 1.0));
}