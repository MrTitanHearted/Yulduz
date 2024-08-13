#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    std::uint32_t GetTextureFormatSize(TextureFormat format) {
        static const std::unordered_map<TextureFormat, size_t> formatSizes{
            {TextureFormat::Undefined, 0},
            {TextureFormat::R8Unorm, 1},
            {TextureFormat::R8Snorm, 1},
            {TextureFormat::R8Uint, 1},
            {TextureFormat::R8Sint, 1},
            {TextureFormat::R16Uint, 2},
            {TextureFormat::R16Sint, 2},
            {TextureFormat::R16Float, 2},
            {TextureFormat::RG8Unorm, 2},
            {TextureFormat::RG8Snorm, 2},
            {TextureFormat::RG8Uint, 2},
            {TextureFormat::RG8Sint, 2},
            {TextureFormat::R32Float, 4},
            {TextureFormat::R32Uint, 4},
            {TextureFormat::R32Sint, 4},
            {TextureFormat::RG16Uint, 4},
            {TextureFormat::RG16Sint, 4},
            {TextureFormat::RG16Float, 4},
            {TextureFormat::RGBA8Unorm, 4},
            {TextureFormat::RGBA8UnormSrgb, 4},
            {TextureFormat::RGBA8Snorm, 4},
            {TextureFormat::RGBA8Uint, 4},
            {TextureFormat::RGBA8Sint, 4},
            {TextureFormat::BGRA8Unorm, 4},
            {TextureFormat::BGRA8UnormSrgb, 4},
            {TextureFormat::RGB10A2Uint, 4},
            {TextureFormat::RGB10A2Unorm, 4},
            {TextureFormat::RG11B10Ufloat, 4},
            {TextureFormat::RGB9E5Ufloat, 4},
            {TextureFormat::RG32Float, 8},
            {TextureFormat::RG32Uint, 8},
            {TextureFormat::RG32Sint, 8},
            {TextureFormat::RGBA16Uint, 8},
            {TextureFormat::RGBA16Sint, 8},
            {TextureFormat::RGBA16Float, 8},
            {TextureFormat::RGBA32Float, 16},
            {TextureFormat::RGBA32Uint, 16},
            {TextureFormat::RGBA32Sint, 16},
            {TextureFormat::Stencil8, 1},
            {TextureFormat::Depth16Unorm, 2},
            {TextureFormat::Depth24Plus, 3},  // Approximate size
            {TextureFormat::Depth24PlusStencil8, 4},
            {TextureFormat::Depth32Float, 4},
            {TextureFormat::Depth32FloatStencil8, 5},  // Approximate size
            {TextureFormat::BC1RGBAUnorm, 8},          // Approximate size per 4x4 block
            {TextureFormat::BC1RGBAUnormSrgb, 8},
            {TextureFormat::BC2RGBAUnorm, 16},
            {TextureFormat::BC2RGBAUnormSrgb, 16},
            {TextureFormat::BC3RGBAUnorm, 16},
            {TextureFormat::BC3RGBAUnormSrgb, 16},
            {TextureFormat::BC4RUnorm, 8},
            {TextureFormat::BC4RSnorm, 8},
            {TextureFormat::BC5RGUnorm, 16},
            {TextureFormat::BC5RGSnorm, 16},
            {TextureFormat::BC6HRGBUfloat, 16},
            {TextureFormat::BC6HRGBFloat, 16},
            {TextureFormat::BC7RGBAUnorm, 16},
            {TextureFormat::BC7RGBAUnormSrgb, 16},
            {TextureFormat::ETC2RGB8Unorm, 8},
            {TextureFormat::ETC2RGB8UnormSrgb, 8},
            {TextureFormat::ETC2RGB8A1Unorm, 8},
            {TextureFormat::ETC2RGB8A1UnormSrgb, 8},
            {TextureFormat::ETC2RGBA8Unorm, 16},
            {TextureFormat::ETC2RGBA8UnormSrgb, 16},
            {TextureFormat::EACR11Unorm, 8},
            {TextureFormat::EACR11Snorm, 8},
            {TextureFormat::EACRG11Unorm, 16},
            {TextureFormat::EACRG11Snorm, 16},
            {TextureFormat::ASTC4x4Unorm, 16},
            {TextureFormat::ASTC4x4UnormSrgb, 16},
            {TextureFormat::ASTC5x4Unorm, 16},
            {TextureFormat::ASTC5x4UnormSrgb, 16},
            {TextureFormat::ASTC5x5Unorm, 16},
            {TextureFormat::ASTC5x5UnormSrgb, 16},
            {TextureFormat::ASTC6x5Unorm, 16},
            {TextureFormat::ASTC6x5UnormSrgb, 16},
            {TextureFormat::ASTC6x6Unorm, 16},
            {TextureFormat::ASTC6x6UnormSrgb, 16},
            {TextureFormat::ASTC8x5Unorm, 16},
            {TextureFormat::ASTC8x5UnormSrgb, 16},
            {TextureFormat::ASTC8x6Unorm, 16},
            {TextureFormat::ASTC8x6UnormSrgb, 16},
            {TextureFormat::ASTC8x8Unorm, 16},
            {TextureFormat::ASTC8x8UnormSrgb, 16},
            {TextureFormat::ASTC10x5Unorm, 16},
            {TextureFormat::ASTC10x5UnormSrgb, 16},
            {TextureFormat::ASTC10x6Unorm, 16},
            {TextureFormat::ASTC10x6UnormSrgb, 16},
            {TextureFormat::ASTC10x8Unorm, 16},
            {TextureFormat::ASTC10x8UnormSrgb, 16},
            {TextureFormat::ASTC10x10Unorm, 16},
            {TextureFormat::ASTC10x10UnormSrgb, 16},
            {TextureFormat::ASTC12x10Unorm, 16},
            {TextureFormat::ASTC12x10UnormSrgb, 16},
            {TextureFormat::ASTC12x12Unorm, 16},
            {TextureFormat::ASTC12x12UnormSrgb, 16},
        };

        return formatSizes.at(format);
    }

    bool IsTextureFormatSrgb(TextureFormat format) {
        static std::unordered_set<TextureFormat> srgbFormats{
            TextureFormat::RGBA8UnormSrgb,
            TextureFormat::BGRA8UnormSrgb,
            TextureFormat::BC1RGBAUnormSrgb,
            TextureFormat::BC2RGBAUnormSrgb,
            TextureFormat::BC3RGBAUnormSrgb,
            TextureFormat::BC7RGBAUnormSrgb,
            TextureFormat::ETC2RGB8UnormSrgb,
            TextureFormat::ETC2RGB8A1UnormSrgb,
            TextureFormat::ETC2RGBA8UnormSrgb,
            TextureFormat::ASTC4x4UnormSrgb,
            TextureFormat::ASTC5x4UnormSrgb,
            TextureFormat::ASTC5x5UnormSrgb,
            TextureFormat::ASTC6x5UnormSrgb,
            TextureFormat::ASTC6x6UnormSrgb,
            TextureFormat::ASTC8x5UnormSrgb,
            TextureFormat::ASTC8x6UnormSrgb,
            TextureFormat::ASTC8x8UnormSrgb,
            TextureFormat::ASTC10x5UnormSrgb,
            TextureFormat::ASTC10x6UnormSrgb,
            TextureFormat::ASTC10x8UnormSrgb,
            TextureFormat::ASTC10x10UnormSrgb,
            TextureFormat::ASTC12x10UnormSrgb,
            TextureFormat::ASTC12x12UnormSrgb,
        };

        return srgbFormats.find(format) != srgbFormats.end();
    }

    const char *GetAdapterTypeName(AdapterType value) {
        switch (value) {
            case AdapterType::DiscreteGPU:
                return "Discrete GPU";
            case AdapterType::IntegratedGPU:
                return "Integrated GPU";
            case AdapterType::CPU:
                return "CPU";
            default:
                return "Unknown";
        }
    }

    const char *GetBackendTypeName(BackendType value) {
        switch (value) {
            case BackendType::Undefined:
                return "Undefined";
            case BackendType::Null:
                return "Null";
            case BackendType::WebGPU:
                return "WebGPU";
            case BackendType::D3D11:
                return "D3D11";
            case BackendType::D3D12:
                return "D3D12";
            case BackendType::Metal:
                return "Metal";
            case BackendType::Vulkan:
                return "Vulkan";
            case BackendType::OpenGL:
                return "OpenGL";
            case BackendType::OpenGLES:
                return "OpenGL ES";
            default:
                return "Unkown";
        }
    }

    const char *GetPresentModeName(PresentMode value) {
        switch (value) {
            case PresentMode::Fifo:
                return "Fifo";
            case PresentMode::FifoRelaxed:
                return "Fifo Relaxed";
            case PresentMode::Immediate:
                return "Immediate";
            case PresentMode::Mailbox:
                return "Mailbox";
            default:
                return "Uknown";
        }
    }

    const char *GetTextureFormatName(TextureFormat value) {
        static std::unordered_map<TextureFormat, const char *> formatNames{
            {TextureFormat::Undefined, "Undefined"},
            {TextureFormat::R8Unorm, "R8Unorm"},
            {TextureFormat::R8Snorm, "R8Snorm"},
            {TextureFormat::R8Uint, "R8Uint"},
            {TextureFormat::R8Sint, "R8Sint"},
            {TextureFormat::R16Uint, "R16Uint"},
            {TextureFormat::R16Sint, "R16Sint"},
            {TextureFormat::R16Float, "R16Float"},
            {TextureFormat::RG8Unorm, "RG8Unorm"},
            {TextureFormat::RG8Snorm, "RG8Snorm"},
            {TextureFormat::RG8Uint, "RG8Uint"},
            {TextureFormat::RG8Sint, "RG8Sint"},
            {TextureFormat::R32Float, "R32Float"},
            {TextureFormat::R32Uint, "R32Uint"},
            {TextureFormat::R32Sint, "R32Sint"},
            {TextureFormat::RG16Uint, "RG16Uint"},
            {TextureFormat::RG16Sint, "RG16Sint"},
            {TextureFormat::RG16Float, "RG16Float"},
            {TextureFormat::RGBA8Unorm, "RGBA8Unorm"},
            {TextureFormat::RGBA8UnormSrgb, "RGBA8UnormSrgb"},
            {TextureFormat::RGBA8Snorm, "RGBA8Snorm"},
            {TextureFormat::RGBA8Uint, "RGBA8Uint"},
            {TextureFormat::RGBA8Sint, "RGBA8Sint"},
            {TextureFormat::BGRA8Unorm, "BGRA8Unorm"},
            {TextureFormat::BGRA8UnormSrgb, "BGRA8UnormSrgb"},
            {TextureFormat::RGB10A2Uint, "RGB10A2Uint"},
            {TextureFormat::RGB10A2Unorm, "RGB10A2Unorm"},
            {TextureFormat::RG11B10Ufloat, "RG11B10Ufloat"},
            {TextureFormat::RGB9E5Ufloat, "RGB9E5Ufloat"},
            {TextureFormat::RG32Float, "RG32Float"},
            {TextureFormat::RG32Uint, "RG32Uint"},
            {TextureFormat::RG32Sint, "RG32Sint"},
            {TextureFormat::RGBA16Uint, "RGBA16Uint"},
            {TextureFormat::RGBA16Sint, "RGBA16Sint"},
            {TextureFormat::RGBA16Float, "RGBA16Float"},
            {TextureFormat::RGBA32Float, "RGBA32Float"},
            {TextureFormat::RGBA32Uint, "RGBA32Uint"},
            {TextureFormat::RGBA32Sint, "RGBA32Sint"},
            {TextureFormat::Stencil8, "Stencil8"},
            {TextureFormat::Depth16Unorm, "Depth16Unorm"},
            {TextureFormat::Depth24Plus, "Depth24Plus"},
            {TextureFormat::Depth24PlusStencil8, "Depth24PlusStencil8"},
            {TextureFormat::Depth32Float, "Depth32Float"},
            {TextureFormat::Depth32FloatStencil8, "Depth32FloatStencil8"},
            {TextureFormat::BC1RGBAUnorm, "BC1RGBAUnorm"},
            {TextureFormat::BC1RGBAUnormSrgb, "BC1RGBAUnormSrgb"},
            {TextureFormat::BC2RGBAUnorm, "BC2RGBAUnorm"},
            {TextureFormat::BC2RGBAUnormSrgb, "BC2RGBAUnormSrgb"},
            {TextureFormat::BC3RGBAUnorm, "BC3RGBAUnorm"},
            {TextureFormat::BC3RGBAUnormSrgb, "BC3RGBAUnormSrgb"},
            {TextureFormat::BC4RUnorm, "BC4RUnorm"},
            {TextureFormat::BC4RSnorm, "BC4RSnorm"},
            {TextureFormat::BC5RGUnorm, "BC5RGUnorm"},
            {TextureFormat::BC5RGSnorm, "BC5RGSnorm"},
            {TextureFormat::BC6HRGBUfloat, "BC6HRGBUfloat"},
            {TextureFormat::BC6HRGBFloat, "BC6HRGBFloat"},
            {TextureFormat::BC7RGBAUnorm, "BC7RGBAUnorm"},
            {TextureFormat::BC7RGBAUnormSrgb, "BC7RGBAUnormSrgb"},
            {TextureFormat::ETC2RGB8Unorm, "ETC2RGB8Unorm"},
            {TextureFormat::ETC2RGB8UnormSrgb, "ETC2RGB8UnormSrgb"},
            {TextureFormat::ETC2RGB8A1Unorm, "ETC2RGB8A1Unorm"},
            {TextureFormat::ETC2RGB8A1UnormSrgb, "ETC2RGB8A1UnormSrgb"},
            {TextureFormat::ETC2RGBA8Unorm, "ETC2RGBA8Unorm"},
            {TextureFormat::ETC2RGBA8UnormSrgb, "ETC2RGBA8UnormSrgb"},
            {TextureFormat::EACR11Unorm, "EACR11Unorm"},
            {TextureFormat::EACR11Snorm, "EACR11Snorm"},
            {TextureFormat::EACRG11Unorm, "EACRG11Unorm"},
            {TextureFormat::EACRG11Snorm, "EACRG11Snorm"},
            {TextureFormat::ASTC4x4Unorm, "ASTC4x4Unorm"},
            {TextureFormat::ASTC4x4UnormSrgb, "ASTC4x4UnormSrgb"},
            {TextureFormat::ASTC5x4Unorm, "ASTC5x4Unorm"},
            {TextureFormat::ASTC5x4UnormSrgb, "ASTC5x4UnormSrgb"},
            {TextureFormat::ASTC5x5Unorm, "ASTC5x5Unorm"},
            {TextureFormat::ASTC5x5UnormSrgb, "ASTC5x5UnormSrgb"},
            {TextureFormat::ASTC6x5Unorm, "ASTC6x5Unorm"},
            {TextureFormat::ASTC6x5UnormSrgb, "ASTC6x5UnormSrgb"},
            {TextureFormat::ASTC6x6Unorm, "ASTC6x6Unorm"},
            {TextureFormat::ASTC6x6UnormSrgb, "ASTC6x6UnormSrgb"},
            {TextureFormat::ASTC8x5Unorm, "ASTC8x5Unorm"},
            {TextureFormat::ASTC8x5UnormSrgb, "ASTC8x5UnormSrgb"},
            {TextureFormat::ASTC8x6Unorm, "ASTC8x6Unorm"},
            {TextureFormat::ASTC8x6UnormSrgb, "ASTC8x6UnormSrgb"},
            {TextureFormat::ASTC8x8Unorm, "ASTC8x8Unorm"},
            {TextureFormat::ASTC8x8UnormSrgb, "ASTC8x8UnormSrgb"},
            {TextureFormat::ASTC10x5Unorm, "ASTC10x5Unorm"},
            {TextureFormat::ASTC10x5UnormSrgb, "ASTC10x5UnormSrgb"},
            {TextureFormat::ASTC10x6Unorm, "ASTC10x6Unorm"},
            {TextureFormat::ASTC10x6UnormSrgb, "ASTC10x6UnormSrgb"},
            {TextureFormat::ASTC10x8Unorm, "ASTC10x8Unorm"},
            {TextureFormat::ASTC10x8UnormSrgb, "ASTC10x8UnormSrgb"},
            {TextureFormat::ASTC10x10Unorm, "ASTC10x10Unorm"},
            {TextureFormat::ASTC10x10UnormSrgb, "ASTC10x10UnormSrgb"},
            {TextureFormat::ASTC12x10Unorm, "ASTC12x10Unorm"},
            {TextureFormat::ASTC12x10UnormSrgb, "ASTC12x10UnormSrgb"},
            {TextureFormat::ASTC12x12Unorm, "ASTC12x12Unorm"},
            {TextureFormat::ASTC12x12UnormSrgb, "ASTC12x12UnormSrgb"},
        };

        return formatNames.at(value);
    }

    /*

    const char *GetBlendFactorName(BlendFactor value) {
        switch (value) {
            case BlendFactor::Zero:
                return "Zero";
            case BlendFactor::One:
                return "One";
            case BlendFactor::Src:
                return "Src";
            case BlendFactor::OneMinusSrc:
                return "One Minus Src";
            case BlendFactor::SrcAlpha:
                return "Src Alpha";
            case BlendFactor::OneMinusSrcAlpha:
                return "One Minus Src Alpha";
            case BlendFactor::Dst:
                return "Dst";
            case BlendFactor::OneMinusDst:
                return "One Minus Dst";
            case BlendFactor::DstAlpha:
                return "Dst Alpha";
            case BlendFactor::OneMinusDstAlpha:
                return "One Minus Dst Alpha";
            case BlendFactor::SrcAlphaSaturated:
                return "Src Alpha Saturated";
            case BlendFactor::Constant:
                return "Constant";
            case BlendFactor::OneMinusConstant:
                return "One Minus Constant";
        }
    }

    const char *GetBlendOperationName(BlendOperation value) {
        switch (value) {
            case BlendOperation::Add:
                return "Add";
            case BlendOperation::Subtract:
                return "Subtract";
            case BlendOperation::ReverseSubtract:
                return "Reverse Subtract";
            case BlendOperation::Min:
                return "Min";
            case BlendOperation::Max:
                return "Max";
        }
    }

    const char *GetBufferBindingTypeName(BufferBindingType value) {
        switch (value) {
            case BufferBindingType::Undefined:
                return "Undefined";
            case BufferBindingType::Uniform:
                return "Uniform";
            case BufferBindingType::Storage:
                return "Storage";
            case BufferBindingType::ReadOnlyStorage:
                return "Read Only Storage";
        }
    }

    const char *GetBufferMapAsyncStatusName(BufferMapAsyncStatus value) {
        switch (value) {
            case BufferMapAsyncStatus::Success:
                return "Success";
            case BufferMapAsyncStatus::ValidationError:
                return "Validation Error";
            case BufferMapAsyncStatus::Unknown:
                return "Unknown";
            case BufferMapAsyncStatus::DeviceLost:
                return "Device Lost";
            case BufferMapAsyncStatus::DestroyedBeforeCallback:
                return "Destroyed Before Callback";
            case BufferMapAsyncStatus::UnmappedBeforeCallback:
                return "Unmapped Before Callback";
            case BufferMapAsyncStatus::MappingAlreadyPending:
                return "Mapping Already Pending";
            case BufferMapAsyncStatus::OffsetOutOfRange:
                return "Offset Out Of Range";
            case BufferMapAsyncStatus::SizeOutOfRange:
                return "Size Out Of Range";
        }
    }
    const char *GetBufferMapStateName(BufferMapState value) {
        switch (value) {
            case BufferMapState::Unmapped:
                return "Unmapped";
            case BufferMapState::Pending:
                return "Pending";
            case BufferMapState::Mapped:
                return "Mapped";
        }
    }

    const char *GetCompareFunctionName(CompareFunction value) {
        switch (value) {
            case CompareFunction::Undefined:
                return "Undefined";
            case CompareFunction::Never:
                return "Never";
            case CompareFunction::Less:
                return "Less";
            case CompareFunction::LessEqual:
                return "Less Equal";
            case CompareFunction::Greater:
                return "Greater";
            case CompareFunction::GreaterEqual:
                return "Greater Equal";
            case CompareFunction::Equal:
                return "Equal";
            case CompareFunction::NotEqual:
                return "Not Equal";
            case CompareFunction::Always:
                return "Always";
        }
    }

    const char *GetCompilationInfoRequestStatusName(CompilationInfoRequestStatus value) {
        switch (value) {
            case CompilationInfoRequestStatus::Success:
                "Success";
            case CompilationInfoRequestStatus::Error:
                "Error";
            case CompilationInfoRequestStatus::DeviceLost:
                "Device Lost";
            case CompilationInfoRequestStatus::Unknown:
                "Unknown";
        }
    }
    const char *GetCompilationMessageTypeName(CompilationMessageType value) {
        switch (value) {
            case CompilationMessageType::Error:
                return "Error";
            case CompilationMessageType::Warning:
                return "Warning";
            case CompilationMessageType::Info:
                return "Info";
        }
    }

    const char *GetCompositeAlphaModeName(CompositeAlphaMode value) {
        switch (value) {
            case CompositeAlphaMode::Auto:
                return "Auto";
            case CompositeAlphaMode::Opaque:
                return "Opaque";
            case CompositeAlphaMode::Premultiplied:
                return "Premultiplied";
            case CompositeAlphaMode::Unpremultiplied:
                return "Unpremultiplied";
            case CompositeAlphaMode::Inherit:
                return "Inherit";
        }
    }

    const char *GetCreatePipelineAsyncStatusName(CreatePipelineAsyncStatus value) {
        switch (value) {
            case CreatePipelineAsyncStatus::Success:
                return "Success";
            case CreatePipelineAsyncStatus::ValidationError:
                return "Validation Error";
            case CreatePipelineAsyncStatus::InternalError:
                return "Internal Error";
            case CreatePipelineAsyncStatus::DeviceLost:
                return "Device Lost";
            case CreatePipelineAsyncStatus::DeviceDestroyed:
                return "Device Destroyed";
            case CreatePipelineAsyncStatus::Unknown:
                return "Unknown";
        }
    }

    const char *GetCullModeName(CullMode value) {
        switch (value) {
            case CullMode::None:
                return "None";
            case CullMode::Front:
                return "Front";
            case CullMode::Back:
                return "Back";
        }
    }

    const char *GetDeviceLostReasonName(DeviceLostReason value) {
        switch (value) {
            case DeviceLostReason::Undefined:
                return "Undefined";
            case DeviceLostReason::Destroyed:
                return "Destroyed";
        }
    }

    const char *GetErrorFilterName(ErrorFilter value) {
        switch (value) {
            case ErrorFilter::Validation:
                return "Validation";
            case ErrorFilter::OutOfMemory:
                return "Out Of Memory";
            case ErrorFilter::Internal:
                return "Internal";
        }
    }

    const char *GetErrorTypeName(ErrorType value) {
        switch (value) {
            case ErrorType::NoError:
                return "No Error";
            case ErrorType::Validation:
                return "Validation";
            case ErrorType::OutOfMemory:
                return "Out Of Memory";
            case ErrorType::Internal:
                return "Internal";
            case ErrorType::Unknown:
                return "Unknown";
            case ErrorType::DeviceLost:
                return "Device Lost";
        }
    }

    const char *GetFeatureNameName(FeatureName value) {
        switch (value) {
            case FeatureName::Undefined:
                return "Undefined";
            case FeatureName::DepthClipControl:
                return "Depth Clip Control";
            case FeatureName::Depth32FloatStencil8:
                return "Depth32 Float Stencil8";
            case FeatureName::TimestampQuery:
                return "Timestamp Query";
            case FeatureName::TextureCompressionBC:
                return "Texture Compression BC";
            case FeatureName::TextureCompressionETC2:
                return "Texture Compression ETC2";
            case FeatureName::TextureCompressionASTC:
                return "Texture Compression ASTC";
            case FeatureName::IndirectFirstInstance:
                return "Indirect First Instance";
            case FeatureName::ShaderF16:
                return "Shader F16";
            case FeatureName::RG11B10UfloatRenderable:
                return "RG11B10Ufloat Renderable";
            case FeatureName::BGRA8UnormStorage:
                return "BGRA8Unorm Storage";
            case FeatureName::Float32Filterable:
                return "Float32 Filterable";
        }
    }

    const char *GetFilterModeName(FilterMode value) {
        switch (value) {
            case FilterMode::Nearest:
                return "Nearest";
            case FilterMode::Linear:
                return "Linear";
        }
    }

    const char *GetFrontFaceName(FrontFace value) {
        switch (value) {
            case FrontFace::CCW:
                return "Counter Clockwise";
            case FrontFace::CW:
                return "Clockwise";
        }
    }

    const char *GetIndexFormatName(IndexFormat value) {
        switch (value) {
            case IndexFormat::Undefined:
                return "Undefined";
            case IndexFormat::Uint16:
                return "Uint16";
            case IndexFormat::Uint32:
                return "Uint32";
        }
    }

    const char *GetLoadOpName(LoadOp value) {
        switch (value) {
            case LoadOp::Undefined:
                return "Undefined";
            case LoadOp::Clear:
                return "Clear";
            case LoadOp::Load:
                return "Load";
        }
    }

    const char *GetMipmapFilterModeName(MipmapFilterMode value) {
        switch (value) {
            case MipmapFilterMode::Nearest:
                return "Nearest";
            case MipmapFilterMode::Linear:
                return "Linear";
        }
    }

    const char *GetPowerPreferenceName(PowerPreference value) {
        switch (value) {
            case PowerPreference::Undefined:
                return "Undefined";
            case PowerPreference::LowPower:
                return "Low Power";
            case PowerPreference::HighPerformance:
                return "High Performance";
        }
    }

    const char *GetPrimitiveTopologyName(PrimitiveTopology value) {
        switch (value) {
            case PrimitiveTopology::PointList:
                return "Point List";
            case PrimitiveTopology::LineList:
                return "Line List";
            case PrimitiveTopology::LineStrip:
                return "Line Strip";
            case PrimitiveTopology::TriangleList:
                return "Triangle List";
            case PrimitiveTopology::TriangleStrip:
                return "Triangle Strip";
        }
    }

    const char *GetQueryTypeName(QueryType value) {
        switch (value) {
            case QueryType::Occlusion:
                return "Occlusion";
            case QueryType::Timestamp:
                return "Timestamp";
        }
    }

    const char *GetQueueWorkDoneStatusName(QueueWorkDoneStatus value) {
        switch (value) {
            case QueueWorkDoneStatus::Success:
                return "Success";
            case QueueWorkDoneStatus::Error:
                return "Error";
            case QueueWorkDoneStatus::Unknown:
                return "Unknown";
            case QueueWorkDoneStatus::DeviceLost:
                return "Device Lost";
        }
    }

    const char *GetRequestAdapterStatusName(RequestAdapterStatus value) {
        switch (value) {
            case RequestAdapterStatus::Success:
                return "Success";
            case RequestAdapterStatus::Unavailable:
                return "Unavailable";
            case RequestAdapterStatus::Error:
                return "Error";
            case RequestAdapterStatus::Unknown:
                return "Unknown";
        }
    }

    const char *GetRequestDeviceStatusName(RequestDeviceStatus value) {
        switch (value) {
            case RequestDeviceStatus::Success:
                return "Success";
            case RequestDeviceStatus::Error:
                return "Error";
            case RequestDeviceStatus::Unknown:
                return "Unknown";
        }
    }

    const char *GetSTypeName(SType value) {
        switch (value) {
            case SType::Invalid:
                "Invalid";
            case SType::SurfaceDescriptorFromMetalLayer:
                "Surface Descriptor From Metal Layer";
            case SType::SurfaceDescriptorFromWindowsHWND:
                "Surface Descriptor From Windows HWND";
            case SType::SurfaceDescriptorFromXlibWindow:
                "Surface Descriptor From Xlib Window";
            case SType::SurfaceDescriptorFromCanvasHTMLSelector:
                "Surface Descriptor From Canvas HTML Selector";
            case SType::ShaderModuleSPIRVDescriptor:
                "Shader Module SPIRV Descriptor";
            case SType::ShaderModuleWGSLDescriptor:
                "Shader Module WGSL Descriptor";
            case SType::PrimitiveDepthClipControl:
                "Primitive Depth Clip Control";
            case SType::SurfaceDescriptorFromWaylandSurface:
                "Surface Descriptor From Wayland Surface";
            case SType::SurfaceDescriptorFromAndroidNativeWindow:
                "Surface Descriptor From Android Native Window";
            case SType::SurfaceDescriptorFromXcbWindow:
                "Surface Descriptor From Xcb Window";
            case SType::RenderPassDescriptorMaxDrawCount:
                "Render Pass Descriptor Max Draw Count";
        }
    }

    const char *GetSamplerAddressModeName(SamplerAddressMode value) {
        switch (value) {
            case SamplerAddressMode::Repeat:
                return "Repeat";
            case SamplerAddressMode::MirrorRepeat:
                return "Mirror Repeat";
            case SamplerAddressMode::ClampToEdge:
                return "Clamp To Edge";
        }
    }
    const char *GetSamplerBindingTypeName(SamplerBindingType value);
    const char *GetStencilOperationName(StencilOperation value);
    const char *GetStorageTextureAccessName(StorageTextureAccess value);
    const char *GetStoreOpName(StoreOp value);
    const char *GetSurfaceGetCurrentTextureStatusName(SurfaceGetCurrentTextureStatus value);
    const char *GetTextureAspectName(TextureAspect value);
    const char *GetTextureDimensionName(TextureDimension value);
    const char *GetTextureFormatName(TextureFormat value);
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
