const MAX_F32: f32 = 3.4028235e+38;
const PI: f32 = 3.141592653589793;

struct FrameData {
    deltaTime: f32,
    time: f32,
}

@group(0) @binding(0) var frame: texture_storage_2d<rgba8unorm, read_write>;
@group(0) @binding(1) var<uniform> frameData: FrameData;

@compute
@workgroup_size(16, 16, 1)
fn cs_main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let frameSize = textureDimensions(frame);
    let uv = vec2<f32>(f32(gid.x) / f32(frameSize.x), f32(gid.y) / f32(frameSize.y));

    var seed = gid.x + gid.y * frameSize.x;
    seed *= u32(frameData.time);

    let color = randomInUnitSphere(&seed) / 2.0 + 0.5;

    textureStore(frame, gid.xy, vec4<f32>(color, 1.0));
}

fn pcg(n: u32) -> u32 {
    var h = n * 747796405u + 2891336453u;
    h = ((h >> ((h >> 28u) + 4u)) ^ h) * 277803737u;
    return (h >> 22u) ^ h;
}

fn pcgf32(seed: ptr<function, u32>) -> f32 {
    *seed = pcg(*seed);
    return f32(*seed) / f32(0xFFFFFFFFu);
}

fn randomInUnitSphere(seed: ptr<function, u32>) -> vec3<f32> {
    let u = pcgf32(seed);
    let v = pcgf32(seed);

    let theta = 2.0 * PI * u;
    let phi = acos(2.0 * v - 1);

    return normalize(vec3<f32>(
        sin(phi) * cos(theta),
        sin(phi) * sin(theta),
        cos(phi),
    ));
}