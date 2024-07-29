#pragma once

#include <Yulduz/Core.hpp>

namespace Yulduz {
    enum class BufferUsage : WGPUFlags {
        None = WGPUBufferUsage_None,
        MapRead = WGPUBufferUsage_MapRead,
        MapWrite = WGPUBufferUsage_MapWrite,
        CopySrc = WGPUBufferUsage_CopySrc,
        CopyDst = WGPUBufferUsage_CopyDst,
        Index = WGPUBufferUsage_Index,
        Vertex = WGPUBufferUsage_Vertex,
        Uniform = WGPUBufferUsage_Uniform,
        Storage = WGPUBufferUsage_Storage,
        Indirect = WGPUBufferUsage_Indirect,
        QueryResolve = WGPUBufferUsage_QueryResolve,
        Force32 = WGPUBufferUsage_Force32,
    };

    enum class IndexFormat {
        Undefined = WGPUIndexFormat_Undefined,
        Uint16 = WGPUIndexFormat_Uint16,
        Uint32 = WGPUIndexFormat_Uint32,
        Force32 = WGPUIndexFormat_Force32,
    };

    enum class RenderContextBackend : WGPUFlags {
        All = WGPUInstanceBackend_All,
        Vulkan = WGPUInstanceBackend_Vulkan,
        GL = WGPUInstanceBackend_GL,
        Metal = WGPUInstanceBackend_Metal,
        DX12 = WGPUInstanceBackend_DX12,
        DX11 = WGPUInstanceBackend_DX11,
        BrowserWebGPU = WGPUInstanceBackend_BrowserWebGPU,
        Primary = WGPUInstanceBackend_Primary,
        Secondary = WGPUInstanceBackend_Secondary,
        Force32 = WGPUInstanceBackend_Force32,
    };

    enum class StencilOperation {
        Keep = WGPUStencilOperation_Keep,
        Zero = WGPUStencilOperation_Zero,
        Replace = WGPUStencilOperation_Replace,
        Invert = WGPUStencilOperation_Invert,
        IncrementClamp = WGPUStencilOperation_IncrementClamp,
        DecrementClamp = WGPUStencilOperation_DecrementClamp,
        IncrementWrap = WGPUStencilOperation_IncrementWrap,
        DecrementWrap = WGPUStencilOperation_DecrementWrap,
        Force32 = WGPUStencilOperation_Force32,
    };

    enum class PrimitiveTopology {
        PointList = WGPUPrimitiveTopology_PointList,
        LineList = WGPUPrimitiveTopology_LineList,
        LineStrip = WGPUPrimitiveTopology_LineStrip,
        TriangleList = WGPUPrimitiveTopology_TriangleList,
        TriangleStrip = WGPUPrimitiveTopology_TriangleStrip,
        Force32 = WGPUPrimitiveTopology_Force32,
    };

    enum class FrontFace {
        CCW = WGPUFrontFace_CCW,
        CW = WGPUFrontFace_CW,
        Force32 = WGPUFrontFace_Force32,
    };

    enum class CullMode {
        None = WGPUCullMode_None,
        Front = WGPUCullMode_Front,
        Back = WGPUCullMode_Back,
        Force32 = WGPUCullMode_Force32,
    };

    enum class LoadOp {
        Undefined = WGPULoadOp_Undefined,
        Clear = WGPULoadOp_Clear,
        Load = WGPULoadOp_Load,
        Force32 = WGPULoadOp_Force32,
    };

    enum class StoreOp {
        Undefined = WGPUStoreOp_Undefined,
        Store = WGPUStoreOp_Store,
        Discard = WGPUStoreOp_Discard,
        Force32 = WGPUStoreOp_Force32,
    };

    enum class AddressMode {
        Repeat = WGPUAddressMode_Repeat,
        MirrorRepeat = WGPUAddressMode_MirrorRepeat,
        ClampToEdge = WGPUAddressMode_ClampToEdge,
        Force32 = WGPUAddressMode_Force32,
    };

    enum class FilterMode {
        Nearest = WGPUFilterMode_Nearest,
        Linear = WGPUFilterMode_Linear,
        Force32 = WGPUFilterMode_Force32,
    };

    enum class MipmapFilterMode {
        Nearest = WGPUMipmapFilterMode_Nearest,
        Linear = WGPUMipmapFilterMode_Linear,
        Force32 = WGPUMipmapFilterMode_Force32,
    };

    enum class CompareFunction {
        Undefined = WGPUCompareFunction_Undefined,
        Never = WGPUCompareFunction_Never,
        Less = WGPUCompareFunction_Less,
        LessEqual = WGPUCompareFunction_LessEqual,
        Greater = WGPUCompareFunction_Greater,
        GreaterEqual = WGPUCompareFunction_GreaterEqual,
        Equal = WGPUCompareFunction_Equal,
        NotEqual = WGPUCompareFunction_NotEqual,
        Always = WGPUCompareFunction_Always,
        Force32 = WGPUCompareFunction_Force32,
    };

    enum class ShaderStage : WGPUFlags {
        Vertex = WGPUShaderStage_Vertex,
        Fragment = WGPUShaderStage_Fragment,
        Compute = WGPUShaderStage_Compute,
        None = WGPUShaderStage_None,
        Force32 = WGPUShaderStage_Force32,
    };

    enum class TextureFormat {
        Undefined = WGPUTextureFormat_Undefined,
        R8Unorm = WGPUTextureFormat_R8Unorm,
        R8Snorm = WGPUTextureFormat_R8Snorm,
        R8Uint = WGPUTextureFormat_R8Uint,
        R8Sint = WGPUTextureFormat_R8Sint,
        R16Uint = WGPUTextureFormat_R16Uint,
        R16Sint = WGPUTextureFormat_R16Sint,
        R16Float = WGPUTextureFormat_R16Float,
        RG8Unorm = WGPUTextureFormat_RG8Unorm,
        RG8Snorm = WGPUTextureFormat_RG8Snorm,
        RG8Uint = WGPUTextureFormat_RG8Uint,
        RG8Sint = WGPUTextureFormat_RG8Sint,
        R32Float = WGPUTextureFormat_R32Float,
        R32Uint = WGPUTextureFormat_R32Uint,
        R32Sint = WGPUTextureFormat_R32Sint,
        RG16Uint = WGPUTextureFormat_RG16Uint,
        RG16Sint = WGPUTextureFormat_RG16Sint,
        RG16Float = WGPUTextureFormat_RG16Float,
        RGBA8Unorm = WGPUTextureFormat_RGBA8Unorm,
        RGBA8UnormSrgb = WGPUTextureFormat_RGBA8UnormSrgb,
        RGBA8Snorm = WGPUTextureFormat_RGBA8Snorm,
        RGBA8Uint = WGPUTextureFormat_RGBA8Uint,
        RGBA8Sint = WGPUTextureFormat_RGBA8Sint,
        BGRA8Unorm = WGPUTextureFormat_BGRA8Unorm,
        BGRA8UnormSrgb = WGPUTextureFormat_BGRA8UnormSrgb,
        RGB10A2Uint = WGPUTextureFormat_RGB10A2Uint,
        RGB10A2Unorm = WGPUTextureFormat_RGB10A2Unorm,
        RG11B10Ufloat = WGPUTextureFormat_RG11B10Ufloat,
        RGB9E5Ufloat = WGPUTextureFormat_RGB9E5Ufloat,
        RG32Float = WGPUTextureFormat_RG32Float,
        RG32Uint = WGPUTextureFormat_RG32Uint,
        RG32Sint = WGPUTextureFormat_RG32Sint,
        RGBA16Uint = WGPUTextureFormat_RGBA16Uint,
        RGBA16Sint = WGPUTextureFormat_RGBA16Sint,
        RGBA16Float = WGPUTextureFormat_RGBA16Float,
        RGBA32Float = WGPUTextureFormat_RGBA32Float,
        RGBA32Uint = WGPUTextureFormat_RGBA32Uint,
        RGBA32Sint = WGPUTextureFormat_RGBA32Sint,
        Stencil8 = WGPUTextureFormat_Stencil8,
        Depth16Unorm = WGPUTextureFormat_Depth16Unorm,
        Depth24Plus = WGPUTextureFormat_Depth24Plus,
        Depth24PlusStencil8 = WGPUTextureFormat_Depth24PlusStencil8,
        Depth32Float = WGPUTextureFormat_Depth32Float,
        Depth32FloatStencil8 = WGPUTextureFormat_Depth32FloatStencil8,
        BC1RGBAUnorm = WGPUTextureFormat_BC1RGBAUnorm,
        BC1RGBAUnormSrgb = WGPUTextureFormat_BC1RGBAUnormSrgb,
        BC2RGBAUnorm = WGPUTextureFormat_BC2RGBAUnorm,
        BC2RGBAUnormSrgb = WGPUTextureFormat_BC2RGBAUnormSrgb,
        BC3RGBAUnorm = WGPUTextureFormat_BC3RGBAUnorm,
        BC3RGBAUnormSrgb = WGPUTextureFormat_BC3RGBAUnormSrgb,
        BC4RUnorm = WGPUTextureFormat_BC4RUnorm,
        BC4RSnorm = WGPUTextureFormat_BC4RSnorm,
        BC5RGUnorm = WGPUTextureFormat_BC5RGUnorm,
        BC5RGSnorm = WGPUTextureFormat_BC5RGSnorm,
        BC6HRGBUfloat = WGPUTextureFormat_BC6HRGBUfloat,
        BC6HRGBFloat = WGPUTextureFormat_BC6HRGBFloat,
        BC7RGBAUnorm = WGPUTextureFormat_BC7RGBAUnorm,
        BC7RGBAUnormSrgb = WGPUTextureFormat_BC7RGBAUnormSrgb,
        ETC2RGB8Unorm = WGPUTextureFormat_ETC2RGB8Unorm,
        ETC2RGB8UnormSrgb = WGPUTextureFormat_ETC2RGB8UnormSrgb,
        ETC2RGB8A1Unorm = WGPUTextureFormat_ETC2RGB8A1Unorm,
        ETC2RGB8A1UnormSrgb = WGPUTextureFormat_ETC2RGB8A1UnormSrgb,
        ETC2RGBA8Unorm = WGPUTextureFormat_ETC2RGBA8Unorm,
        ETC2RGBA8UnormSrgb = WGPUTextureFormat_ETC2RGBA8UnormSrgb,
        EACR11Unorm = WGPUTextureFormat_EACR11Unorm,
        EACR11Snorm = WGPUTextureFormat_EACR11Snorm,
        EACRG11Unorm = WGPUTextureFormat_EACRG11Unorm,
        EACRG11Snorm = WGPUTextureFormat_EACRG11Snorm,
        ASTC4x4Unorm = WGPUTextureFormat_ASTC4x4Unorm,
        ASTC4x4UnormSrgb = WGPUTextureFormat_ASTC4x4UnormSrgb,
        ASTC5x4Unorm = WGPUTextureFormat_ASTC5x4Unorm,
        ASTC5x4UnormSrgb = WGPUTextureFormat_ASTC5x4UnormSrgb,
        ASTC5x5Unorm = WGPUTextureFormat_ASTC5x5Unorm,
        ASTC5x5UnormSrgb = WGPUTextureFormat_ASTC5x5UnormSrgb,
        ASTC6x5Unorm = WGPUTextureFormat_ASTC6x5Unorm,
        ASTC6x5UnormSrgb = WGPUTextureFormat_ASTC6x5UnormSrgb,
        ASTC6x6Unorm = WGPUTextureFormat_ASTC6x6Unorm,
        ASTC6x6UnormSrgb = WGPUTextureFormat_ASTC6x6UnormSrgb,
        ASTC8x5Unorm = WGPUTextureFormat_ASTC8x5Unorm,
        ASTC8x5UnormSrgb = WGPUTextureFormat_ASTC8x5UnormSrgb,
        ASTC8x6Unorm = WGPUTextureFormat_ASTC8x6Unorm,
        ASTC8x6UnormSrgb = WGPUTextureFormat_ASTC8x6UnormSrgb,
        ASTC8x8Unorm = WGPUTextureFormat_ASTC8x8Unorm,
        ASTC8x8UnormSrgb = WGPUTextureFormat_ASTC8x8UnormSrgb,
        ASTC10x5Unorm = WGPUTextureFormat_ASTC10x5Unorm,
        ASTC10x5UnormSrgb = WGPUTextureFormat_ASTC10x5UnormSrgb,
        ASTC10x6Unorm = WGPUTextureFormat_ASTC10x6Unorm,
        ASTC10x6UnormSrgb = WGPUTextureFormat_ASTC10x6UnormSrgb,
        ASTC10x8Unorm = WGPUTextureFormat_ASTC10x8Unorm,
        ASTC10x8UnormSrgb = WGPUTextureFormat_ASTC10x8UnormSrgb,
        ASTC10x10Unorm = WGPUTextureFormat_ASTC10x10Unorm,
        ASTC10x10UnormSrgb = WGPUTextureFormat_ASTC10x10UnormSrgb,
        ASTC12x10Unorm = WGPUTextureFormat_ASTC12x10Unorm,
        ASTC12x10UnormSrgb = WGPUTextureFormat_ASTC12x10UnormSrgb,
        ASTC12x12Unorm = WGPUTextureFormat_ASTC12x12Unorm,
        ASTC12x12UnormSrgb = WGPUTextureFormat_ASTC12x12UnormSrgb,
        Force32 = WGPUTextureFormat_Force32,
    };

    enum class TextureUsage : WGPUFlags {
        None = WGPUTextureUsage_None,
        CopySrc = WGPUTextureUsage_CopySrc,
        CopyDst = WGPUTextureUsage_CopyDst,
        TextureBinding = WGPUTextureUsage_TextureBinding,
        StorageBinding = WGPUTextureUsage_StorageBinding,
        RenderAttachment = WGPUTextureUsage_RenderAttachment,
        Force32 = WGPUTextureUsage_Force32,
    };

    enum class TextureDimension {
        D1 = WGPUTextureDimension_1D,
        D2 = WGPUTextureDimension_2D,
        D3 = WGPUTextureDimension_3D,
        Force32 = WGPUTextureDimension_Force32,
    };

    enum class TextureViewDimension {
        D1 = WGPUTextureViewDimension_1D,
        D2 = WGPUTextureViewDimension_2D,
        D2Array = WGPUTextureViewDimension_2DArray,
        D3 = WGPUTextureViewDimension_3D,
        Cube = WGPUTextureViewDimension_Cube,
        CubeArray = WGPUTextureViewDimension_CubeArray,
        Force32 = WGPUTextureViewDimension_Force32,
    };

    enum class VertexFormat {
        Undefined = WGPUVertexFormat_Undefined,
        Uint8x2 = WGPUVertexFormat_Uint8x2,
        Uint8x4 = WGPUVertexFormat_Uint8x4,
        Sint8x2 = WGPUVertexFormat_Sint8x2,
        Sint8x4 = WGPUVertexFormat_Sint8x4,
        Unorm8x2 = WGPUVertexFormat_Unorm8x2,
        Unorm8x4 = WGPUVertexFormat_Unorm8x4,
        Snorm8x2 = WGPUVertexFormat_Snorm8x2,
        Snorm8x4 = WGPUVertexFormat_Snorm8x4,
        Uint16x2 = WGPUVertexFormat_Uint16x2,
        Uint16x4 = WGPUVertexFormat_Uint16x4,
        Sint16x2 = WGPUVertexFormat_Sint16x2,
        Sint16x4 = WGPUVertexFormat_Sint16x4,
        Unorm16x2 = WGPUVertexFormat_Unorm16x2,
        Unorm16x4 = WGPUVertexFormat_Unorm16x4,
        Snorm16x2 = WGPUVertexFormat_Snorm16x2,
        Snorm16x4 = WGPUVertexFormat_Snorm16x4,
        Float16x2 = WGPUVertexFormat_Float16x2,
        Float16x4 = WGPUVertexFormat_Float16x4,
        Float32 = WGPUVertexFormat_Float32,
        Float32x2 = WGPUVertexFormat_Float32x2,
        Float32x3 = WGPUVertexFormat_Float32x3,
        Float32x4 = WGPUVertexFormat_Float32x4,
        Uint32 = WGPUVertexFormat_Uint32,
        Uint32x2 = WGPUVertexFormat_Uint32x2,
        Uint32x3 = WGPUVertexFormat_Uint32x3,
        Uint32x4 = WGPUVertexFormat_Uint32x4,
        Sint32 = WGPUVertexFormat_Sint32,
        Sint32x2 = WGPUVertexFormat_Sint32x2,
        Sint32x3 = WGPUVertexFormat_Sint32x3,
        Sint32x4 = WGPUVertexFormat_Sint32x4,
        Force32 = WGPUVertexFormat_Force32,
    };

    enum class VertexStepMode {
        Vertex = WGPUVertexStepMode_Vertex,
        Instance = WGPUVertexStepMode_Instance,
        VertexBufferNotUsed = WGPUVertexStepMode_VertexBufferNotUsed,
        Force32 = WGPUVertexStepMode_Force32,
    };

    enum class PresentMode {
        Fifo = WGPUPresentMode_Fifo,
        FifoRelaxed = WGPUPresentMode_FifoRelaxed,
        Immediate = WGPUPresentMode_Immediate,
        Mailbox = WGPUPresentMode_Mailbox,
        Force32 = WGPUPresentMode_Force32,
    };

    enum class CompositeAlphaMode {
        Auto = WGPUCompositeAlphaMode_Auto,
        Opaque = WGPUCompositeAlphaMode_Opaque,
        Premultiplied = WGPUCompositeAlphaMode_Premultiplied,
        Unpremultiplied = WGPUCompositeAlphaMode_Unpremultiplied,
        Inherit = WGPUCompositeAlphaMode_Inherit,
        Force32 = WGPUCompositeAlphaMode_Force32,
    };

    enum class PowerPreference {
        Undefined = WGPUPowerPreference_Undefined,
        LowPower = WGPUPowerPreference_LowPower,
        HighPerformance = WGPUPowerPreference_HighPerformance,
        Force32 = WGPUPowerPreference_Force32,
    };

    enum class BufferBindingType {
        Undefined = WGPUBufferBindingType_Undefined,
        Uniform = WGPUBufferBindingType_Uniform,
        Storage = WGPUBufferBindingType_Storage,
        ReadOnlyStorage = WGPUBufferBindingType_ReadOnlyStorage,
        Force32 = WGPUBufferBindingType_Force32,
    };

    enum class SamplerBindingType {
        Undefined = WGPUSamplerBindingType_Undefined,
        Filtering = WGPUSamplerBindingType_Filtering,
        NonFiltering = WGPUSamplerBindingType_NonFiltering,
        Comparison = WGPUSamplerBindingType_Comparison,
        Force32 = WGPUSamplerBindingType_Force32,
    };

    enum class TextureSampleType {
        Undefined = WGPUTextureSampleType_Undefined,
        Float = WGPUTextureSampleType_Float,
        UnfilterableFloat = WGPUTextureSampleType_UnfilterableFloat,
        Depth = WGPUTextureSampleType_Depth,
        Sint = WGPUTextureSampleType_Sint,
        Uint = WGPUTextureSampleType_Uint,
        Force32 = WGPUTextureSampleType_Force32,
    };

    enum class StorageTextureAccess {
        Undefined = WGPUStorageTextureAccess_Undefined,
        WriteOnly = WGPUStorageTextureAccess_WriteOnly,
        ReadOnly = WGPUStorageTextureAccess_ReadOnly,
        ReadWrite = WGPUStorageTextureAccess_ReadWrite,
        Force32 = WGPUStorageTextureAccess_Force32,
    };

    enum class BlendOperation {
        Add = WGPUBlendOperation_Add,
        Subtract = WGPUBlendOperation_Subtract,
        ReverseSubtract = WGPUBlendOperation_ReverseSubtract,
        Min = WGPUBlendOperation_Min,
        Max = WGPUBlendOperation_Max,
        Force32 = WGPUBlendOperation_Force32,
    };

    enum class BlendFactor {
        Zero = WGPUBlendFactor_Zero,
        One = WGPUBlendFactor_One,
        Src = WGPUBlendFactor_Src,
        OneMinusSrc = WGPUBlendFactor_OneMinusSrc,
        SrcAlpha = WGPUBlendFactor_SrcAlpha,
        OneMinusSrcAlpha = WGPUBlendFactor_OneMinusSrcAlpha,
        Dst = WGPUBlendFactor_Dst,
        OneMinusDst = WGPUBlendFactor_OneMinusDst,
        DstAlpha = WGPUBlendFactor_DstAlpha,
        OneMinusDstAlpha = WGPUBlendFactor_OneMinusDstAlpha,
        SrcAlphaSaturated = WGPUBlendFactor_SrcAlphaSaturated,
        Constant = WGPUBlendFactor_Constant,
        OneMinusConstant = WGPUBlendFactor_OneMinusConstant,
        Force32 = WGPUBlendFactor_Force32,
    };

    enum class ColorWriteMask : WGPUFlags {
        None = WGPUColorWriteMask_None,
        Red = WGPUColorWriteMask_Red,
        Green = WGPUColorWriteMask_Green,
        Blue = WGPUColorWriteMask_Blue,
        Alpha = WGPUColorWriteMask_Alpha,
        All = WGPUColorWriteMask_All,
        Force32 = WGPUColorWriteMask_Force32,
    };

    enum class WebGPULogLevel {
        Off = WGPULogLevel_Off,
        Error = WGPULogLevel_Error,
        Warn = WGPULogLevel_Warn,
        Info = WGPULogLevel_Info,
        Debug = WGPULogLevel_Debug,
        Trace = WGPULogLevel_Trace,
        Force32 = WGPULogLevel_Force32,
    };

    enum class TextureAspect {
        All = WGPUTextureAspect_All,
        StencilOnly = WGPUTextureAspect_StencilOnly,
        DepthOnly = WGPUTextureAspect_DepthOnly,
        Force32 = WGPUTextureAspect_Force32,
    };

    enum class CameraMovement {
        Forward = 1 << 0,
        Backward = 1 << 1,
        Up = 1 << 2,
        Down = 1 << 3,
        WorldForward = 1 << 4,
        WorldBackward = 1 << 5,
        WorldUp = 1 << 6,
        WorldDown = 1 << 7,
        Right = 1 << 8,
        Left = 1 << 9,
    };

    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(RenderContextBackend);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(TextureUsage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(BufferUsage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(ShaderStage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(ColorWriteMask);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(CameraMovement);
}  // namespace Yulduz