#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace ioport_emu {

enum class IoDirection {
    input,
    output,
};

enum class PortSource {
    immediate8,
    dx,
};

enum class OperandWidth : std::uint8_t {
    bits8 = 8,
    bits16 = 16,
    bits32 = 32,
};

struct DecodedInstruction {
    IoDirection direction;
    PortSource port_source;
    OperandWidth operand_width;
    std::uint16_t port;
    std::uint8_t opcode;
    std::uint8_t length;
    bool has_operand_size_prefix;
};

std::optional<DecodedInstruction> decode_instruction(
    const std::uint8_t* instruction,
    std::size_t instruction_size,
    std::uint16_t dx
);

std::uint32_t operand_mask(OperandWidth width);

} // namespace ioport_emu
