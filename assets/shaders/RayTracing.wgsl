const MAX_F32: f32 = 3.4028235e+38;
const PI: f32 = 3.141592653589793;

struct Camera {
    InverseProjection: mat4x4<f32>,
    InverseView: mat4x4<f32>,
    Position: vec3<f32>,
    Time: f32,
    SkyColor: vec3<f32>,
    AddSky: u32,
    FrameIndex: u32,
    Bounces: u32,
}

struct Material {
    Albedo: vec3<f32>,
    Metallic: f32,
    EmissionColor: vec3<f32>,
    EmissionPower: f32,
    Roughness: f32,
}

struct Sphere {
    Position: vec3<f32>,
    Radius: f32,
    MaterialIndex: u32,
}

struct Ray {
    Origin: vec3<f32>,
    Direction: vec3<f32>,
}

@group(0) @binding(0) var<uniform> camera: Camera;

@group(1) @binding(0) var<storage, read_write> materials: array<Material>;
@group(1) @binding(1) var<storage, read_write> spheres: array<Sphere>;

@group(2) @binding(0) var accumulatedFramebuffer: texture_storage_2d<rgba32float, read_write>;
@group(2) @binding(1) var framebuffer: texture_storage_2d<rgba8unorm, write>;

@compute
@workgroup_size(16, 16, 1)
fn cs_main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let frameSize = textureDimensions(framebuffer);
    let uv = vec2<f32>(f32(gid.x) / f32(frameSize.x), f32(gid.y) / f32(frameSize.y));
    let coord = uv * 2.0 - 1.0;

    let targetPixel = camera.InverseProjection * vec4<f32>(coord.x, coord.y, 1, 1);
    let rayDirection = (camera.InverseView * vec4<f32>(normalize(targetPixel.xyz / targetPixel.w), 0.0)).xyz;

    var ray: Ray;
    ray.Origin = camera.Position;
    ray.Direction = rayDirection;

    var light = vec3<f32>(0.0);
    var contribution = vec3<f32>(1.0);

    var seed = gid.x + gid.y * frameSize.x;
    seed *= camera.FrameIndex * u32(camera.Time * 1000.0);

    for (var i: u32 = 0u; i < camera.Bounces; i++) {
        seed += i;

        let payload = traceRay(ray);
        if payload.HitDistance < 0 {
            if (camera.AddSky > 0) {
                light += camera.SkyColor * contribution;
            }
            break;
        }

        let sphere = spheres[payload.ObjectIndex];
        let material = materials[sphere.MaterialIndex];

        contribution *= material.Albedo;
        light += material.EmissionColor * material.EmissionPower;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.00001;
        ray.Direction = normalize(payload.WorldNormal + randomInUnitSphere(&seed));
    }

    let color = vec4<f32>(light, 1.0);

    if (camera.FrameIndex == 1) {
        textureStore(accumulatedFramebuffer, gid.xy, color);
        textureStore(framebuffer, gid.xy, color);
        return;
    }

    var accumulatedColor = textureLoad(accumulatedFramebuffer, gid.xy);
    accumulatedColor += color;
    textureStore(accumulatedFramebuffer, gid.xy, accumulatedColor);

    accumulatedColor /= f32(camera.FrameIndex);

    textureStore(framebuffer, gid.xy, accumulatedColor);
}

struct HitPayload {
    HitDistance: f32,

    WorldPosition: vec3<f32>,
    WorldNormal: vec3<f32>,

    ObjectIndex: i32,
}

fn traceRay(ray: Ray) -> HitPayload {
    var closestSphere = -1;
    var hitDistance = MAX_F32;

    for (var i: u32 = 0; i < arrayLength(&spheres); i++) {
        let sphere = spheres[i];

        let rs = ray.Origin - sphere.Position;
        let a = dot(ray.Direction, ray.Direction);
        let bHalf = dot(ray.Direction, rs);
        let c = dot(rs, rs) - sphere.Radius * sphere.Radius;
        let determinant = bHalf * bHalf - a * c;

        if determinant < 0 {
            continue;
        }

        let t = (-bHalf - sqrt(determinant)) / a;

        if hitDistance > t && t > 0.0 {
            hitDistance = t;
            closestSphere = i32(i);
        }
    }

    if closestSphere < 0 {
        return miss(ray);
    }

    return closestHit(ray, hitDistance, closestSphere);
}

fn closestHit(ray: Ray, hitDistance: f32, objectIndex: i32) -> HitPayload {
    let sphere = spheres[objectIndex];

    var payload: HitPayload;

    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    payload.WorldPosition = ray.Origin + ray.Direction * hitDistance;
    payload.WorldNormal = normalize(payload.WorldPosition - sphere.Position);

    return payload;
}

fn miss(ray: Ray) -> HitPayload {
    var payload: HitPayload;
    payload.HitDistance = -1.0;
    return payload;
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
    return normalize(vec3<f32>(
        pcgf32(seed) * 2.0 - 1.0,
        pcgf32(seed) * 2.0 - 1.0,
        pcgf32(seed) * 2.0 - 1.0,
    ));
}