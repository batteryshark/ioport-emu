# IOPortEmu

IOPortEmu emulates x86 port I/O from user space. The idea: let legacy software
run privileged `in` and `out` instructions, catch the fault the CPU or OS raises,
and dispatch the port operation to an emulator callback instead of real hardware.

This is archival code, not an actively maintained hardware interface. The
maintained part is a small C++17 core that decodes I/O instructions and routes
them to handlers. The original platform-hook experiments live under `legacy/` so
the history stays visible without making the top level look more finished than it
is.

## Repository map

| Path | Purpose |
| --- | --- |
| `include/ioport_emu/` | Public C++17 headers for instruction decoding and handler dispatch. |
| `src/` | Implementation of the decoder, handler registry, and emulator facade. |
| `tests/` | Characterization tests for decoding and dispatch. |
| `legacy/v1-c-prototype/` | Original C proof of concept with GCC/MinGW scripts. |
| `legacy/cpp-exception-hook-sketch/` | Earlier, unfinished C++ exception-hook rewrite, kept as reference. |

## Quick start

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

To pin a compiler explicitly, pass it to the configure step, for example
`-G Ninja -DCMAKE_CXX_COMPILER=clang++`.

## Supported instructions

The decoder recognizes the single-byte port I/O opcodes, with an optional `0x66`
operand-size prefix.

| Opcode | Form | Port source | Width | Width with `0x66` |
| --- | --- | --- | --- | --- |
| `0xE4` | `IN AL, imm8` | Immediate 8-bit | 8-bit | 8-bit |
| `0xE5` | `IN EAX/AX, imm8` | Immediate 8-bit | 32-bit | 16-bit |
| `0xEC` | `IN AL, DX` | `DX` | 8-bit | 8-bit |
| `0xED` | `IN EAX/AX, DX` | `DX` | 32-bit | 16-bit |
| `0xE6` | `OUT imm8, AL` | Immediate 8-bit | 8-bit | 8-bit |
| `0xE7` | `OUT imm8, EAX/AX` | Immediate 8-bit | 32-bit | 16-bit |
| `0xEE` | `OUT DX, AL` | `DX` | 8-bit | 8-bit |
| `0xEF` | `OUT DX, EAX/AX` | `DX` | 32-bit | 16-bit |

## How it works

1. A caller passes instruction bytes and the current `XAX` and `XDX` register
   values to `IOPortEmulator::handle_instruction`.
2. `decode_instruction` matches one of the opcodes above.
3. Input operations route to `HandlerRegistry::read`, output operations to
   `HandlerRegistry::write`. Missing input handlers return `0`; missing output
   handlers are no-ops.
4. An input handler's return value replaces only the operand-width portion of
   `XAX`. An output handler receives the masked value that would have gone to the
   port.
5. `handle_instruction` returns the number of instruction bytes a platform hook
   would add to the instruction pointer.

## Design

The maintained core decodes instructions and dispatches handlers without touching
any OS API, so it builds and tests without executing privileged instructions. The
process-global exception and signal hooks that make the emulation trick work are
kept out of the core deliberately: they are x86- and register-layout-specific,
and POSIX signal handlers cannot safely call arbitrary heap-backed callbacks. A
future revival should translate platform fault context into the core emulator API
rather than wiring the legacy hooks back in unchanged.

## Legacy

`legacy/v1-c-prototype/` is the original working C implementation and its
GCC/MinGW test harness. `legacy/cpp-exception-hook-sketch/` is an unfinished C++
rewrite that once sat at the repository root. Both are reference material, not a
supported runtime layer, and their scripts may need local toolchain adjustment
before they build on a modern machine.
