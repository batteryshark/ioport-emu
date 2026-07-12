#include "ioport_emu/IOPortEmulator.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

namespace {

using ioport_emu::DecodedInstruction;
using ioport_emu::IOPortEmulator;
using ioport_emu::IoDirection;
using ioport_emu::OperandWidth;
using ioport_emu::PortSource;
using ioport_emu::RegisterValue;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

DecodedInstruction require_decoded(
    const std::optional<DecodedInstruction>& decoded,
    const std::string& context
)
{
    require(decoded.has_value(), context + " should decode");
    return *decoded;
}

void decodes_immediate_input()
{
    const std::uint8_t instruction[] = {0xE4, 0x60};
    const auto decoded = require_decoded(
        ioport_emu::decode_instruction(instruction, sizeof(instruction), 0),
        "IN AL, imm8"
    );

    require(decoded.direction == IoDirection::input, "IN AL, imm8 direction");
    require(decoded.port_source == PortSource::immediate8, "IN AL, imm8 port source");
    require(decoded.operand_width == OperandWidth::bits8, "IN AL, imm8 width");
    require(decoded.port == 0x60, "IN AL, imm8 port");
    require(decoded.length == 2, "IN AL, imm8 length");
}

void decodes_operand_size_prefix()
{
    const std::uint8_t instruction[] = {0x66, 0xE5, 0x60};
    const auto decoded = require_decoded(
        ioport_emu::decode_instruction(instruction, sizeof(instruction), 0),
        "IN AX, imm8"
    );

    require(decoded.direction == IoDirection::input, "IN AX, imm8 direction");
    require(decoded.operand_width == OperandWidth::bits16, "IN AX, imm8 width");
    require(decoded.has_operand_size_prefix, "IN AX, imm8 prefix");
    require(decoded.length == 3, "IN AX, imm8 length");
}

void decodes_dx_output()
{
    const std::uint8_t instruction[] = {0xEF};
    const auto decoded = require_decoded(
        ioport_emu::decode_instruction(instruction, sizeof(instruction), 0x0376),
        "OUT DX, EAX"
    );

    require(decoded.direction == IoDirection::output, "OUT DX, EAX direction");
    require(decoded.port_source == PortSource::dx, "OUT DX, EAX port source");
    require(decoded.operand_width == OperandWidth::bits32, "OUT DX, EAX width");
    require(decoded.port == 0x0376, "OUT DX, EAX port");
    require(decoded.length == 1, "OUT DX, EAX length");
}

void rejects_unknown_or_truncated_instructions()
{
    const std::uint8_t nop[] = {0x90};
    const std::uint8_t truncated[] = {0xE4};

    require(!ioport_emu::decode_instruction(nop, sizeof(nop), 0), "NOP should not decode");
    require(!ioport_emu::decode_instruction(truncated, sizeof(truncated), 0), "truncated IN should not decode");
}

void dispatches_input_and_preserves_unwritten_register_bits()
{
    IOPortEmulator emulator;
    emulator.handlers().register_input_handler(
        0x0060,
        [](auto, auto width, auto current_value) {
            require(width == OperandWidth::bits8, "input handler receives width");
            require(current_value == 0x12345678u, "input handler receives current register value");
            return RegisterValue{0xAB};
        }
    );

    const std::uint8_t instruction[] = {0xE4, 0x60};
    RegisterValue xax = 0x12345678u;
    const auto advance = emulator.handle_instruction(instruction, sizeof(instruction), xax, 0);

    require(advance == 2, "input dispatch advances past instruction");
    require(xax == 0x123456ABu, "8-bit input only replaces AL");
}

void dispatches_output_with_masked_value()
{
    IOPortEmulator emulator;
    bool handler_called = false;

    emulator.handlers().register_output_handler(
        0x0376,
        [&](auto port, auto width, auto value) {
            handler_called = true;
            require(port == 0x0376, "output handler receives DX port");
            require(width == OperandWidth::bits16, "output handler receives prefixed width");
            require(value == 0x5678u, "output handler receives masked value");
        }
    );

    const std::uint8_t instruction[] = {0x66, 0xEF};
    RegisterValue xax = 0x12345678u;
    const auto advance = emulator.handle_instruction(instruction, sizeof(instruction), xax, 0x0376);

    require(handler_called, "output handler is called");
    require(advance == 2, "output dispatch advances past prefixed instruction");
    require(xax == 0x12345678u, "output does not change register value");
}

} // namespace

int main()
{
    decodes_immediate_input();
    decodes_operand_size_prefix();
    decodes_dx_output();
    rejects_unknown_or_truncated_instructions();
    dispatches_input_and_preserves_unwritten_register_bits();
    dispatches_output_with_masked_value();

    std::cout << "ioport_emu tests passed\n";
    return EXIT_SUCCESS;
}
