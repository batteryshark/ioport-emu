#include "ioport_emu/Decoder.h"

namespace ioport_emu {
namespace {

constexpr std::uint8_t operand_size_prefix = 0x66;

constexpr std::uint8_t opcode_out_imm8_al = 0xE6;
constexpr std::uint8_t opcode_out_imm8_xax = 0xE7;
constexpr std::uint8_t opcode_out_dx_al = 0xEE;
constexpr std::uint8_t opcode_out_dx_xax = 0xEF;

constexpr std::uint8_t opcode_in_al_imm8 = 0xE4;
constexpr std::uint8_t opcode_in_xax_imm8 = 0xE5;
constexpr std::uint8_t opcode_in_al_dx = 0xEC;
constexpr std::uint8_t opcode_in_xax_dx = 0xED;

OperandWidth width_for_xax_opcode(bool has_operand_size_prefix)
{
    return has_operand_size_prefix ? OperandWidth::bits16 : OperandWidth::bits32;
}

std::optional<DecodedInstruction> decode_with_immediate_port(
    const std::uint8_t* instruction,
    std::size_t instruction_size,
    std::size_t opcode_index,
    bool has_operand_size_prefix,
    IoDirection direction,
    OperandWidth operand_width
)
{
    const auto length = static_cast<std::uint8_t>(opcode_index + 2);
    if (instruction_size < length) {
        return std::nullopt;
    }

    return DecodedInstruction{
        direction,
        PortSource::immediate8,
        operand_width,
        instruction[opcode_index + 1],
        instruction[opcode_index],
        length,
        has_operand_size_prefix,
    };
}

DecodedInstruction decode_with_dx_port(
    const std::uint8_t* instruction,
    std::size_t opcode_index,
    bool has_operand_size_prefix,
    IoDirection direction,
    OperandWidth operand_width,
    std::uint16_t dx
)
{
    return DecodedInstruction{
        direction,
        PortSource::dx,
        operand_width,
        dx,
        instruction[opcode_index],
        static_cast<std::uint8_t>(opcode_index + 1),
        has_operand_size_prefix,
    };
}

} // namespace

std::optional<DecodedInstruction> decode_instruction(
    const std::uint8_t* instruction,
    std::size_t instruction_size,
    std::uint16_t dx
)
{
    if (instruction == nullptr || instruction_size == 0) {
        return std::nullopt;
    }

    const bool has_operand_size_prefix = instruction[0] == operand_size_prefix;
    const std::size_t opcode_index = has_operand_size_prefix ? 1 : 0;

    if (instruction_size <= opcode_index) {
        return std::nullopt;
    }

    switch (instruction[opcode_index]) {
    case opcode_out_imm8_al:
        return decode_with_immediate_port(
            instruction,
            instruction_size,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::output,
            OperandWidth::bits8
        );
    case opcode_out_imm8_xax:
        return decode_with_immediate_port(
            instruction,
            instruction_size,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::output,
            width_for_xax_opcode(has_operand_size_prefix)
        );
    case opcode_out_dx_al:
        return decode_with_dx_port(
            instruction,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::output,
            OperandWidth::bits8,
            dx
        );
    case opcode_out_dx_xax:
        return decode_with_dx_port(
            instruction,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::output,
            width_for_xax_opcode(has_operand_size_prefix),
            dx
        );
    case opcode_in_al_imm8:
        return decode_with_immediate_port(
            instruction,
            instruction_size,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::input,
            OperandWidth::bits8
        );
    case opcode_in_xax_imm8:
        return decode_with_immediate_port(
            instruction,
            instruction_size,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::input,
            width_for_xax_opcode(has_operand_size_prefix)
        );
    case opcode_in_al_dx:
        return decode_with_dx_port(
            instruction,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::input,
            OperandWidth::bits8,
            dx
        );
    case opcode_in_xax_dx:
        return decode_with_dx_port(
            instruction,
            opcode_index,
            has_operand_size_prefix,
            IoDirection::input,
            width_for_xax_opcode(has_operand_size_prefix),
            dx
        );
    default:
        return std::nullopt;
    }
}

std::uint32_t operand_mask(OperandWidth width)
{
    switch (width) {
    case OperandWidth::bits8:
        return 0x000000FFu;
    case OperandWidth::bits16:
        return 0x0000FFFFu;
    case OperandWidth::bits32:
        return 0xFFFFFFFFu;
    }

    return 0u;
}

} // namespace ioport_emu
