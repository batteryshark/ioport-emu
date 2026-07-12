#pragma once

#include "ioport_emu/Decoder.h"

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace ioport_emu {

using Port = std::uint16_t;
using RegisterValue = std::uint32_t;
using InputHandler = std::function<RegisterValue(Port port, OperandWidth width, RegisterValue current_value)>;
using OutputHandler = std::function<void(Port port, OperandWidth width, RegisterValue value)>;

class HandlerRegistry {
public:
    void register_input_handler(Port port, InputHandler handler);
    void register_output_handler(Port port, OutputHandler handler);

    bool remove_input_handler(Port port);
    bool remove_output_handler(Port port);

    bool has_input_handler(Port port) const;
    bool has_output_handler(Port port) const;

    RegisterValue read(Port port, OperandWidth width, RegisterValue current_value) const;
    void write(Port port, OperandWidth width, RegisterValue value) const;

private:
    std::unordered_map<Port, InputHandler> input_handlers_;
    std::unordered_map<Port, OutputHandler> output_handlers_;
};

} // namespace ioport_emu
