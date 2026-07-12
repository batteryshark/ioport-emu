# Legacy v1 C Prototype

This is the original C proof of concept for intercepting x86 `in` and `out`
instructions from user space by catching the exception raised by privileged I/O
instructions.

It is preserved as historical reference. The scripts are intentionally close to
the original GCC/MinGW workflow and may need local toolchain adjustment before
they build on a modern machine.

The maintained archival rewrite lives at the repository root and keeps the
instruction decoding and handler dispatch testable without executing privileged
instructions.
