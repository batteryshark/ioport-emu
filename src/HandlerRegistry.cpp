#include "ioport_emu/HandlerRegistry.h"

#include <stdexcept>
#include <utility>

namespace ioport_emu {

void HandlerRegistry::register_input_handler(Port port, InputHandler handler)
{
    if (!handler) {
        throw std::invalid_argument("input handler must be callable");
    }

    input_handlers_[port] = std::move(handler);
}

void HandlerRegistry::register_output_handler(Port port, OutputHandler handler)
{
    if (!handler) {
        throw std::invalid_argument("output handler must be callable");
    }

    output_handlers_[port] = std::move(handler);
}

bool HandlerRegistry::remove_input_handler(Port port)
{
    return input_handlers_.erase(port) > 0;
}

bool HandlerRegistry::remove_output_handler(Port port)
{
    return output_handlers_.erase(port) > 0;
}

bool HandlerRegistry::has_input_handler(Port port) const
{
    return input_handlers_.find(port) != input_handlers_.end();
}

bool HandlerRegistry::has_output_handler(Port port) const
{
    return output_handlers_.find(port) != output_handlers_.end();
}

RegisterValue HandlerRegistry::read(Port port, OperandWidth width, RegisterValue current_value) const
{
    const auto handler = input_handlers_.find(port);
    if (handler == input_handlers_.end()) {
        return 0;
    }

    return handler->second(port, width, current_value);
}

void HandlerRegistry::write(Port port, OperandWidth width, RegisterValue value) const
{
    const auto handler = output_handlers_.find(port);
    if (handler == output_handlers_.end()) {
        return;
    }

    handler->second(port, width, value);
}

} // namespace ioport_emu
