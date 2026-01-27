struct VertexInput {
    [[vk::location(0)]]
    float3 Position;
    [[vk::location(1)]]
    float3 Color;
};

struct VertexOutput {
    float4 Position : SV_Position;
    [[vk::location(0)]]
    float3 Color;
};

struct Camera {
    float4x4 ProjectionView;
};

[[vk::binding(0, 1)]]
ConstantBuffer<Camera> gCamera;

[shader("vertex")]
VertexOutput vs_main(
    VertexInput input) {
    VertexOutput output;

    output.Position = mul(gCamera.ProjectionView, float4(input.Position, 1.0f));
    output.Color    = input.Color;

    return output;
}

[shader("fragment")]
float4 fs_main(VertexOutput input)
    : SV_Target {
    return float4(input.Color, 1.0f);
}

