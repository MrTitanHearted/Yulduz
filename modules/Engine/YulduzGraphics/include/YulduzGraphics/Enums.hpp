#pragma once

#include <Yulduz/Core.hpp>

namespace Yulduz {
    enum class AdapterType : std::uint32_t {
        DiscreteGPU = WGPUAdapterType_DiscreteGPU,
        IntegratedGPU = WGPUAdapterType_IntegratedGPU,
        CPU = WGPUAdapterType_CPU,
        Unknown = WGPUAdapterType_Unknown,
    };

    enum class AddressMode : std::uint32_t {
        Repeat = WGPUAddressMode_Repeat,
        MirrorRepeat = WGPUAddressMode_MirrorRepeat,
        ClampToEdge = WGPUAddressMode_ClampToEdge,
    };

    enum class BackendType : std::uint32_t {
        Undefined = WGPUBackendType_Undefined,
        Null = WGPUBackendType_Null,
        WebGPU = WGPUBackendType_WebGPU,
        D3D11 = WGPUBackendType_D3D11,
        D3D12 = WGPUBackendType_D3D12,
        Metal = WGPUBackendType_Metal,
        Vulkan = WGPUBackendType_Vulkan,
        OpenGL = WGPUBackendType_OpenGL,
        OpenGLES = WGPUBackendType_OpenGLES,
    };

    enum class BlendFactor : std::uint32_t {
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
    };

    enum class BlendOperation : std::uint32_t {
        Add = WGPUBlendOperation_Add,
        Subtract = WGPUBlendOperation_Subtract,
        ReverseSubtract = WGPUBlendOperation_ReverseSubtract,
        Min = WGPUBlendOperation_Min,
        Max = WGPUBlendOperation_Max,
    };

    enum class BufferBindingType : std::uint32_t {
        Undefined = WGPUBufferBindingType_Undefined,
        Uniform = WGPUBufferBindingType_Uniform,
        Storage = WGPUBufferBindingType_Storage,
        ReadOnlyStorage = WGPUBufferBindingType_ReadOnlyStorage,
    };

    enum class BufferMapAsyncStatus : std::uint32_t {
        Success = WGPUBufferMapAsyncStatus_Success,
        ValidationError = WGPUBufferMapAsyncStatus_ValidationError,
        Unknown = WGPUBufferMapAsyncStatus_Unknown,
        DeviceLost = WGPUBufferMapAsyncStatus_DeviceLost,
        DestroyedBeforeCallback = WGPUBufferMapAsyncStatus_DestroyedBeforeCallback,
        UnmappedBeforeCallback = WGPUBufferMapAsyncStatus_UnmappedBeforeCallback,
        MappingAlreadyPending = WGPUBufferMapAsyncStatus_MappingAlreadyPending,
        OffsetOutOfRange = WGPUBufferMapAsyncStatus_OffsetOutOfRange,
        SizeOutOfRange = WGPUBufferMapAsyncStatus_SizeOutOfRange,
    };

    enum class BufferMapState : std::uint32_t {
        Unmapped = WGPUBufferMapState_Unmapped,
        Pending = WGPUBufferMapState_Pending,
        Mapped = WGPUBufferMapState_Mapped,
    };

    enum class CompareFunction : std::uint32_t {
        Undefined = WGPUCompareFunction_Undefined,
        Never = WGPUCompareFunction_Never,
        Less = WGPUCompareFunction_Less,
        LessEqual = WGPUCompareFunction_LessEqual,
        Greater = WGPUCompareFunction_Greater,
        GreaterEqual = WGPUCompareFunction_GreaterEqual,
        Equal = WGPUCompareFunction_Equal,
        NotEqual = WGPUCompareFunction_NotEqual,
        Always = WGPUCompareFunction_Always,
    };

    enum class CompilationInfoRequestStatus : std::uint32_t {
        Success = WGPUCompilationInfoRequestStatus_Success,
        Error = WGPUCompilationInfoRequestStatus_Error,
        DeviceLost = WGPUCompilationInfoRequestStatus_DeviceLost,
        Unknown = WGPUCompilationInfoRequestStatus_Unknown,
    };

    enum class CompilationMessageType : std::uint32_t {
        Error = WGPUCompilationMessageType_Error,
        Warning = WGPUCompilationMessageType_Warning,
        Info = WGPUCompilationMessageType_Info,
    };

    enum class CompositeAlphaMode : std::uint32_t {
        Auto = WGPUCompositeAlphaMode_Auto,
        Opaque = WGPUCompositeAlphaMode_Opaque,
        Premultiplied = WGPUCompositeAlphaMode_Premultiplied,
        Unpremultiplied = WGPUCompositeAlphaMode_Unpremultiplied,
        Inherit = WGPUCompositeAlphaMode_Inherit,
    };

    enum class CreatePipelineAsyncStatus : std::uint32_t {
        Success = WGPUCreatePipelineAsyncStatus_Success,
        ValidationError = WGPUCreatePipelineAsyncStatus_ValidationError,
        InternalError = WGPUCreatePipelineAsyncStatus_InternalError,
        DeviceLost = WGPUCreatePipelineAsyncStatus_DeviceLost,
        DeviceDestroyed = WGPUCreatePipelineAsyncStatus_DeviceDestroyed,
        Unknown = WGPUCreatePipelineAsyncStatus_Unknown,
    };

    enum class CullMode : std::uint32_t {
        None = WGPUCullMode_None,
        Front = WGPUCullMode_Front,
        Back = WGPUCullMode_Back,
    };

    enum class DeviceLostReason : std::uint32_t {
        Undefined = WGPUDeviceLostReason_Undefined,
        Destroyed = WGPUDeviceLostReason_Destroyed,
    };

    enum class ErrorFilter : std::uint32_t {
        Validation = WGPUErrorFilter_Validation,
        OutOfMemory = WGPUErrorFilter_OutOfMemory,
        Internal = WGPUErrorFilter_Internal,
    };

    enum class ErrorType : std::uint32_t {
        NoError = WGPUErrorType_NoError,
        Validation = WGPUErrorType_Validation,
        OutOfMemory = WGPUErrorType_OutOfMemory,
        Internal = WGPUErrorType_Internal,
        Unknown = WGPUErrorType_Unknown,
        DeviceLost = WGPUErrorType_DeviceLost,
    };

    enum class FeatureName : std::uint32_t {
        Undefined = WGPUFeatureName_Undefined,
        DepthClipControl = WGPUFeatureName_DepthClipControl,
        Depth32FloatStencil8 = WGPUFeatureName_Depth32FloatStencil8,
        TimestampQuery = WGPUFeatureName_TimestampQuery,
        TextureCompressionBC = WGPUFeatureName_TextureCompressionBC,
        TextureCompressionETC2 = WGPUFeatureName_TextureCompressionETC2,
        TextureCompressionASTC = WGPUFeatureName_TextureCompressionASTC,
        IndirectFirstInstance = WGPUFeatureName_IndirectFirstInstance,
        ShaderF16 = WGPUFeatureName_ShaderF16,
        RG11B10UfloatRenderable = WGPUFeatureName_RG11B10UfloatRenderable,
        BGRA8UnormStorage = WGPUFeatureName_BGRA8UnormStorage,
        Float32Filterable = WGPUFeatureName_Float32Filterable,
    };

    enum class FilterMode : std::uint32_t {
        Nearest = WGPUFilterMode_Nearest,
        Linear = WGPUFilterMode_Linear,
    };

    enum class FrontFace : std::uint32_t {
        CCW = WGPUFrontFace_CCW,
        CW = WGPUFrontFace_CW,
    };

    enum class IndexFormat : std::uint32_t {
        Undefined = WGPUIndexFormat_Undefined,
        Uint16 = WGPUIndexFormat_Uint16,
        Uint32 = WGPUIndexFormat_Uint32,
    };

    enum class LoadOp : std::uint32_t {
        Undefined = WGPULoadOp_Undefined,
        Clear = WGPULoadOp_Clear,
        Load = WGPULoadOp_Load,
    };

    enum class MipmapFilterMode : std::uint32_t {
        Nearest = WGPUMipmapFilterMode_Nearest,
        Linear = WGPUMipmapFilterMode_Linear,
    };

    enum class PowerPreference : std::uint32_t {
        Undefined = WGPUPowerPreference_Undefined,
        LowPower = WGPUPowerPreference_LowPower,
        HighPerformance = WGPUPowerPreference_HighPerformance,
    };

    enum class PresentMode : std::uint32_t {
        Fifo = WGPUPresentMode_Fifo,
        FifoRelaxed = WGPUPresentMode_FifoRelaxed,
        Immediate = WGPUPresentMode_Immediate,
        Mailbox = WGPUPresentMode_Mailbox,
    };

    enum class PrimitiveTopology : std::uint32_t {
        PointList = WGPUPrimitiveTopology_PointList,
        LineList = WGPUPrimitiveTopology_LineList,
        LineStrip = WGPUPrimitiveTopology_LineStrip,
        TriangleList = WGPUPrimitiveTopology_TriangleList,
        TriangleStrip = WGPUPrimitiveTopology_TriangleStrip,
    };

    enum class QueryType : std::uint32_t {
        Occlusion = WGPUQueryType_Occlusion,
        Timestamp = WGPUQueryType_Timestamp,
    };

    enum class QueueWorkDoneStatus : std::uint32_t {
        Success = WGPUQueueWorkDoneStatus_Success,
        Error = WGPUQueueWorkDoneStatus_Error,
        Unknown = WGPUQueueWorkDoneStatus_Unknown,
        DeviceLost = WGPUQueueWorkDoneStatus_DeviceLost,
    };

    enum class RequestAdapterStatus : std::uint32_t {
        Success = WGPURequestAdapterStatus_Success,
        Unavailable = WGPURequestAdapterStatus_Unavailable,
        Error = WGPURequestAdapterStatus_Error,
        Unknown = WGPURequestAdapterStatus_Unknown,
    };

    enum class RequestDeviceStatus : std::uint32_t {
        Success = WGPURequestDeviceStatus_Success,
        Error = WGPURequestDeviceStatus_Error,
        Unknown = WGPURequestDeviceStatus_Unknown,
    };

    enum class SType : std::uint32_t {
        Invalid = WGPUSType_Invalid,
        SurfaceDescriptorFromMetalLayer = WGPUSType_SurfaceDescriptorFromMetalLayer,
        SurfaceDescriptorFromWindowsHWND = WGPUSType_SurfaceDescriptorFromWindowsHWND,
        SurfaceDescriptorFromXlibWindow = WGPUSType_SurfaceDescriptorFromXlibWindow,
        SurfaceDescriptorFromCanvasHTMLSelector = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
        ShaderModuleSPIRVDescriptor = WGPUSType_ShaderModuleSPIRVDescriptor,
        ShaderModuleWGSLDescriptor = WGPUSType_ShaderModuleWGSLDescriptor,
        PrimitiveDepthClipControl = WGPUSType_PrimitiveDepthClipControl,
        SurfaceDescriptorFromWaylandSurface = WGPUSType_SurfaceDescriptorFromWaylandSurface,
        SurfaceDescriptorFromAndroidNativeWindow = WGPUSType_SurfaceDescriptorFromAndroidNativeWindow,
        SurfaceDescriptorFromXcbWindow = WGPUSType_SurfaceDescriptorFromXcbWindow,
        RenderPassDescriptorMaxDrawCount = WGPUSType_RenderPassDescriptorMaxDrawCount,
    };

    enum class SamplerAddressMode : std::uint32_t {
        Repeat = WGPUAddressMode_Repeat,
        MirrorRepeat = WGPUAddressMode_MirrorRepeat,
        ClampToEdge = WGPUAddressMode_ClampToEdge,
    };

    enum class SamplerBindingType : std::uint32_t {
        Undefined = WGPUSamplerBindingType_Undefined,
        Filtering = WGPUSamplerBindingType_Filtering,
        NonFiltering = WGPUSamplerBindingType_NonFiltering,
        Comparison = WGPUSamplerBindingType_Comparison,
    };

    enum class StencilOperation : std::uint32_t {
        Keep = WGPUStencilOperation_Keep,
        Zero = WGPUStencilOperation_Zero,
        Replace = WGPUStencilOperation_Replace,
        Invert = WGPUStencilOperation_Invert,
        IncrementClamp = WGPUStencilOperation_IncrementClamp,
        DecrementClamp = WGPUStencilOperation_DecrementClamp,
        IncrementWrap = WGPUStencilOperation_IncrementWrap,
        DecrementWrap = WGPUStencilOperation_DecrementWrap,
    };

    enum class StorageTextureAccess : std::uint32_t {
        Undefined = WGPUStorageTextureAccess_Undefined,
        WriteOnly = WGPUStorageTextureAccess_WriteOnly,
        ReadOnly = WGPUStorageTextureAccess_ReadOnly,
        ReadWrite = WGPUStorageTextureAccess_ReadWrite,
    };

    enum class StoreOp : std::uint32_t {
        Undefined = WGPUStoreOp_Undefined,
        Store = WGPUStoreOp_Store,
        Discard = WGPUStoreOp_Discard,
    };

    enum class SurfaceGetCurrentTextureStatus : std::uint32_t {
        Success = WGPUSurfaceGetCurrentTextureStatus_Success,
        Timeout = WGPUSurfaceGetCurrentTextureStatus_Timeout,
        Outdated = WGPUSurfaceGetCurrentTextureStatus_Outdated,
        Lost = WGPUSurfaceGetCurrentTextureStatus_Lost,
        OutOfMemory = WGPUSurfaceGetCurrentTextureStatus_OutOfMemory,
        DeviceLost = WGPUSurfaceGetCurrentTextureStatus_DeviceLost,
    };

    enum class TextureAspect : std::uint32_t {
        All = WGPUTextureAspect_All,
        StencilOnly = WGPUTextureAspect_StencilOnly,
        DepthOnly = WGPUTextureAspect_DepthOnly,
    };

    enum class TextureDimension : std::uint32_t {
        D1 = WGPUTextureDimension_1D,
        D2 = WGPUTextureDimension_2D,
        D3 = WGPUTextureDimension_3D,
    };

    enum class TextureFormat : std::uint32_t {
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
    };

    enum class TextureSampleType : std::uint32_t {
        Undefined = WGPUTextureSampleType_Undefined,
        Float = WGPUTextureSampleType_Float,
        UnfilterableFloat = WGPUTextureSampleType_UnfilterableFloat,
        Depth = WGPUTextureSampleType_Depth,
        Sint = WGPUTextureSampleType_Sint,
        Uint = WGPUTextureSampleType_Uint,
    };

    enum class TextureViewDimension : std::uint32_t {
        Undefined = WGPUTextureViewDimension_Undefined,
        D1 = WGPUTextureViewDimension_1D,
        D2 = WGPUTextureViewDimension_2D,
        D2Array = WGPUTextureViewDimension_2DArray,
        Cube = WGPUTextureViewDimension_Cube,
        CubeArray = WGPUTextureViewDimension_CubeArray,
        D3 = WGPUTextureViewDimension_3D,
    };

    enum class VertexFormat : std::uint32_t {
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
    };

    enum class VertexStepMode : std::uint32_t {
        Vertex = WGPUVertexStepMode_Vertex,
        Instance = WGPUVertexStepMode_Instance,
        VertexBufferNotUsed = WGPUVertexStepMode_VertexBufferNotUsed,
    };

    enum class BufferUsage : std::uint32_t {
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
    };

    enum class ColorWriteMask : std::uint32_t {
        None = WGPUColorWriteMask_None,
        Red = WGPUColorWriteMask_Red,
        Green = WGPUColorWriteMask_Green,
        Blue = WGPUColorWriteMask_Blue,
        Alpha = WGPUColorWriteMask_Alpha,
        All = WGPUColorWriteMask_All,
    };

    enum class MapMode : std::uint32_t {
        None = WGPUMapMode_None,
        Read = WGPUMapMode_Read,
        Write = WGPUMapMode_Write,
    };

    enum class ShaderStage : std::uint32_t {
        None = WGPUShaderStage_None,
        Vertex = WGPUShaderStage_Vertex,
        Fragment = WGPUShaderStage_Fragment,
        Compute = WGPUShaderStage_Compute,
    };

    enum class TextureUsage : std::uint32_t {
        None = WGPUTextureUsage_None,
        CopySrc = WGPUTextureUsage_CopySrc,
        CopyDst = WGPUTextureUsage_CopyDst,
        TextureBinding = WGPUTextureUsage_TextureBinding,
        StorageBinding = WGPUTextureUsage_StorageBinding,
        RenderAttachment = WGPUTextureUsage_RenderAttachment,
    };

    enum class WGPUNativeSType : std::uint32_t {
        DeviceExtras = WGPUSType_DeviceExtras,
        RequiredLimitsExtras = WGPUSType_RequiredLimitsExtras,
        PipelineLayoutExtras = WGPUSType_PipelineLayoutExtras,
        ShaderModuleGLSLDescriptor = WGPUSType_ShaderModuleGLSLDescriptor,
        SupportedLimitsExtras = WGPUSType_SupportedLimitsExtras,
        InstanceExtras = WGPUSType_InstanceExtras,
        BindGroupEntryExtras = WGPUSType_BindGroupEntryExtras,
        BindGroupLayoutEntryExtras = WGPUSType_BindGroupLayoutEntryExtras,
        QuerySetDescriptorExtras = WGPUSType_QuerySetDescriptorExtras,
        SurfaceConfigurationExtras = WGPUSType_SurfaceConfigurationExtras,
        SType_Force32 = WGPUNativeSType_Force32,
    };

    enum class NativeFeature : std::uint32_t {
        PushConstants = WGPUNativeFeature_PushConstants,
        TextureAdapterSpecificFormatFeatures = WGPUNativeFeature_TextureAdapterSpecificFormatFeatures,
        MultiDrawIndirect = WGPUNativeFeature_MultiDrawIndirect,
        MultiDrawIndirectCount = WGPUNativeFeature_MultiDrawIndirectCount,
        VertexWritableStorage = WGPUNativeFeature_VertexWritableStorage,
        TextureBindingArray = WGPUNativeFeature_TextureBindingArray,
        SampledTextureAndStorageBufferArrayNonUniformIndexing = WGPUNativeFeature_SampledTextureAndStorageBufferArrayNonUniformIndexing,
        PipelineStatisticsQuery = WGPUNativeFeature_PipelineStatisticsQuery,
        StorageResourceBindingArray = WGPUNativeFeature_StorageResourceBindingArray,
        PartiallyBoundBindingArray = WGPUNativeFeature_PartiallyBoundBindingArray,
    };

    enum class WebGPULogLevel : std::uint32_t {
        Off = WGPULogLevel_Off,
        Error = WGPULogLevel_Error,
        Warn = WGPULogLevel_Warn,
        Info = WGPULogLevel_Info,
        Debug = WGPULogLevel_Debug,
        Trace = WGPULogLevel_Trace,
    };

    enum class InstanceBackend : std::uint32_t {
        All = WGPUInstanceBackend_All,
        Vulkan = WGPUInstanceBackend_Vulkan,
        GL = WGPUInstanceBackend_GL,
        Metal = WGPUInstanceBackend_Metal,
        DX12 = WGPUInstanceBackend_DX12,
        DX11 = WGPUInstanceBackend_DX11,
        BrowserWebGPU = WGPUInstanceBackend_BrowserWebGPU,
        Primary = WGPUInstanceBackend_Primary,
        Secondary = WGPUInstanceBackend_Secondary,
    };

    enum class InstanceFlag : std::uint32_t {
        Default = WGPUInstanceFlag_Default,
        Debug = WGPUInstanceFlag_Debug,
        Validation = WGPUInstanceFlag_Validation,
        DiscardHalLabels = WGPUInstanceFlag_DiscardHalLabels,
    };

    enum class Dx12Compiler : std::uint32_t {
        Undefined = WGPUDx12Compiler_Undefined,
        Fxc = WGPUDx12Compiler_Fxc,
        Dxc = WGPUDx12Compiler_Dxc,
    };

    enum class Gles3MinorVersion : std::uint32_t {
        Automatic = WGPUGles3MinorVersion_Automatic,
        Version0 = WGPUGles3MinorVersion_Version0,
        Version1 = WGPUGles3MinorVersion_Version1,
        Version2 = WGPUGles3MinorVersion_Version2,
    };

    enum class PipelineStatisticName : std::uint32_t {
        VertexShaderInvocations = WGPUPipelineStatisticName_VertexShaderInvocations,
        ClipperInvocations = WGPUPipelineStatisticName_ClipperInvocations,
        ClipperPrimitivesOut = WGPUPipelineStatisticName_ClipperPrimitivesOut,
        FragmentShaderInvocations = WGPUPipelineStatisticName_FragmentShaderInvocations,
        ComputeShaderInvocations = WGPUPipelineStatisticName_ComputeShaderInvocations,
    };

    enum class NativeQueryType : std::uint32_t {
        PipelineStatistics = WGPUNativeQueryType_PipelineStatistics,
    };

    enum class CameraMovement : std::uint32_t {
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

    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(InstanceBackend);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(TextureUsage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(BufferUsage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(ShaderStage);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(ColorWriteMask);
    YULDUZ_ENUM_UINT_BITWISE_OPERATORS(CameraMovement);

    std::uint32_t GetTextureFormatSize(TextureFormat format);
    bool IsTextureFormatSrgb(TextureFormat format);

    const char *GetAdapterTypeName(AdapterType value);
    const char *GetBackendTypeName(BackendType value);
    const char *GetPresentModeName(PresentMode value);
    const char *GetTextureFormatName(TextureFormat value);

    /*
    const char *GetBlendFactorName(BlendFactor value);
    const char *GetBlendOperationName(BlendOperation value);
    const char *GetBufferBindingTypeName(BufferBindingType value);
    const char *GetBufferMapAsyncStatusName(BufferMapAsyncStatus value);
    const char *GetBufferMapStateName(BufferMapState value);
    const char *GetCompareFunctionName(CompareFunction value);
    const char *GetCompilationInfoRequestStatusName(CompilationInfoRequestStatus value);
    const char *GetCompilationMessageTypeName(CompilationMessageType value);
    const char *GetCompositeAlphaModeName(CompositeAlphaMode value);
    const char *GetCreatePipelineAsyncStatusName(CreatePipelineAsyncStatus value);
    const char *GetCullModeName(CullMode value);
    const char *GetDeviceLostReasonName(DeviceLostReason value);
    const char *GetErrorFilterName(ErrorFilter value);
    const char *GetErrorTypeName(ErrorType value);
    const char *GetFeatureNameName(FeatureName value);
    const char *GetFilterModeName(FilterMode value);
    const char *GetFrontFaceName(FrontFace value);
    const char *GetIndexFormatName(IndexFormat value);
    const char *GetLoadOpName(LoadOp value);
    const char *GetMipmapFilterModeName(MipmapFilterMode value);
    const char *GetPowerPreferenceName(PowerPreference value);
    const char *GetPrimitiveTopologyName(PrimitiveTopology value);
    const char *GetQueryTypeName(QueryType value);
    const char *GetQueueWorkDoneStatusName(QueueWorkDoneStatus value);
    const char *GetRequestAdapterStatusName(RequestAdapterStatus value);
    const char *GetRequestDeviceStatusName(RequestDeviceStatus value);
    const char *GetSTypeName(SType value);
    const char *GetSamplerAddressModeName(SamplerAddressMode value);
    const char *GetSamplerBindingTypeName(SamplerBindingType value);
    const char *GetStencilOperationName(StencilOperation value);
    const char *GetStorageTextureAccessName(StorageTextureAccess value);
    const char *GetStoreOpName(StoreOp value);
    const char *GetSurfaceGetCurrentTextureStatusName(SurfaceGetCurrentTextureStatus value);
    const char *GetTextureAspectName(TextureAspect value);
    const char *GetTextureDimensionName(TextureDimension value);
    const char *GetTextureSampleTypeName(TextureSampleType value);
    const char *GetTextureViewDimensionName(TextureViewDimension value);
    const char *GetVertexFormatName(VertexFormat value);
    const char *GetVertexStepModeName(VertexStepMode value);
    const char *GetBufferUsageName(BufferUsage value);
    const char *GetColorWriteMaskName(ColorWriteMask value);
    const char *GetMapModeName(MapMode value);
    const char *GetShaderStageName(ShaderStage value);
    const char *GetTextureUsageName(TextureUsage value);
    const char *GetWGPUNativeSTypeName(WGPUNativeSType value);
    const char *GetNativeFeatureName(NativeFeature value);
    const char *GetWebGPULogLevelName(WebGPULogLevel value);
    const char *GetInstanceBackendName(InstanceBackend value);
    const char *GetInstanceFlagName(InstanceFlag value);
    const char *GetDx12CompilerName(Dx12Compiler value);
    const char *GetGles3MinorVersionName(Gles3MinorVersion value);
    const char *GetPipelineStatisticNameName(PipelineStatisticName value);
    const char *GetNativeQueryTypeName(NativeQueryType value);
    const char *GetCameraMovementName(CameraMovement value);
    */
}  // namespace Yulduz
