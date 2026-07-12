#include "ioport_emu/IOPortEmulator.h"

namespace ioport_emu {
namespace {

RegisterValue merge_low_bits(RegisterValue original, RegisterValue replacement, OperandWidth width)
{
    const RegisterValue mask = operand_mask(width);
    return (original & ~mask) | (replacement & mask);
}

} // namespace

HandlerRegistry& IOPortEmulator::handlers()
{
    return handlers_;
}

const HandlerRegistry& IOPortEmulator::handlers() const
{
    return handlers_;
}

std::optional<std::uint8_t> IOPortEmulator::handle_instruction(
    const std::uint8_t* instruction,
    std::size_t instruction_size,
    RegisterValue& xax,
    RegisterValue xdx
) const
{
    const auto decoded = decode_instruction(
        instruction,
        instruction_size,
        static_cast<std::uint16_t>(xdx)
    );

    if (!decoded) {
        return std::nullopt;
    }

    if (decoded->direction == IoDirection::input) {
        const RegisterValue value = handlers_.read(decoded->port, decoded->operand_width, xax);
        xax = merge_low_bits(xax, value, decoded->operand_width);
    } else {
        handlers_.write(decoded->port, decoded->operand_width, xax & operand_mask(decoded->operand_width));
    }

    return decoded->length;
}

} // namespace ioport_emu
