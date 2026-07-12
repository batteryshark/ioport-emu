#pragma once

#include "ioport_emu/HandlerRegistry.h"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace ioport_emu {

class IOPortEmulator {
public:
    HandlerRegistry& handlers();
    const HandlerRegistry& handlers() const;

    std::optional<std::uint8_t> handle_instruction(
        const std::uint8_t* instruction,
        std::size_t instruction_size,
        RegisterValue& xax,
        RegisterValue xdx
    ) const;

private:
    HandlerRegistry handlers_;
};

} // namespace ioport_emu
