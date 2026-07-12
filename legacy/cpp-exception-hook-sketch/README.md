# Legacy C++ Exception-Hook Sketch

This directory preserves the unfinished C++ rewrite that used to sit at the
repository root.

It is useful historical context, but it should not be treated as the maintained
implementation. The sketch still mixes platform exception APIs, object lifetime,
instruction decoding, and handler dispatch. It also relies on member-function
callbacks where C-style exception and signal handlers need a different boundary.

The maintained archival rewrite at the repository root keeps decoding and
dispatch separate from any future platform hook.
