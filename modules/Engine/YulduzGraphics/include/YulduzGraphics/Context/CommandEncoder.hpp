#pragma once

#include <Yulduz/Core.hpp>

namespace Yulduz {
    class GraphicsContext;

    class CommandBuffer {
       public:
        CommandBuffer(const WGPUCommandBuffer &handle);
        ~CommandBuffer();

        CommandBuffer(const CommandBuffer &other);
        CommandBuffer &operator=(const CommandBuffer &other);

        CommandBuffer(CommandBuffer &&other);
        CommandBuffer &operator=(CommandBuffer &&other);

        WGPUCommandBuffer get() const;

       private:
        WGPUCommandBuffer m_CommandBuffer;
    };

    class CommandEncoder {
       public:
        CommandEncoder(const std::string &label, const WGPUCommandEncoder &handle);
        ~CommandEncoder();

        CommandEncoder(const CommandEncoder &other);
        CommandEncoder &operator=(const CommandEncoder &other);

        CommandEncoder(CommandEncoder &&other);
        CommandEncoder &operator=(CommandEncoder &&other);

        WGPUCommandEncoder get() const;
        std::string getLabel() const;

        CommandBuffer finish(const std::string &label = "Yulduz Command Buffer");

       private:
        std::string m_Label;
        WGPUCommandEncoder m_CommandEncoder;
    };

    class CommandEncoderBuilder {
       public:
        CommandEncoderBuilder();
        ~CommandEncoderBuilder() = default;

        inline static CommandEncoderBuilder New() { return CommandEncoderBuilder(); }

        CommandEncoderBuilder &setLabel(const std::string &label);

        CommandEncoder build(const GraphicsContext &context);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz
