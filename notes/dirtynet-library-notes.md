# dirtyNet Library Notes

These are library-design notes for dirtyNet itself. Keep raw C socket learning
notes in `notes/udp-pingpong-library-notes.md`; keep reusable API, packet,
framing, and test-format decisions here.

## Packet Framing And Header Size

The current local TCP packet sandbox may use `sizeof(header)` as a convenient
learning shortcut while everything is compiled and run on the same machine.

When this becomes a library feature or a real test-packet format, do not make
the C++ `header` object layout be the wire format. Track the encoded header size
explicitly instead.

Reasons:

- C++ structs can contain padding bytes.
- Field order and object layout are implementation details unless deliberately
  controlled.
- Header fields may change without meaning the wire format should change.
- Multi-byte integers still need explicit host/network byte-order conversion.
- Tests should assert the exact encoded byte count and field offsets, not
  whatever `sizeof(header)` happens to be.

Preferred future pattern:

- Define a constant encoded header size, such as `encoded_header_size`.
- Encode each header field into the buffer at a known offset.
- Decode each header field from the same explicit offsets.
- Treat the payload length as data in the wire header, not as a property
  inferred from the C++ struct size.

## Application Boundary From TCP Packet Sandbox

The TCP packet sandbox is teaching application-layer shape, not defining the
core dirtyNet protocol. Do not turn the sandbox `packet` concept directly into
the primary library abstraction.

dirtyNet's core goal is to be the socket layer used by application protocols:
connection setup, listening, accepting, reading, writing, descriptor ownership,
EOF handling, error reporting, and efficient byte movement.

Application code should own packet meaning:

- packet or message types
- payload legality, such as which message kinds can carry payload bytes
- schema and version decisions
- command semantics, such as ping, pong, greeting, or quit
- decode policy for malformed application data

The library may provide convenience packet or framing helpers, but those should
be optional utilities layered over the same socket operations. They should help
users get rolling without making dirtyNet itself an application protocol.

## Buffer IO As The Source Of Truth

The most fundamental read and write APIs should be byte-buffer oriented and
highly optimized.

Possible core operations:

- read into caller-provided byte storage
- write from caller-provided byte storage
- read some bytes when available
- read exactly a requested byte count when the caller needs that behavior
- write until all bytes are sent, or report the partial/error condition

This layer should expose real transport behavior instead of hiding it behind
application assumptions. For TCP in particular, a read returning zero bytes
means the peer closed the connection; it does not mean an empty application
message or an empty network buffer.

Keep TCP stream behavior explicit:

- TCP preserves byte order, not application message boundaries.
- A single application message can arrive across multiple reads.
- Multiple application writes can be observed as one read.
- dirtyNet can provide helpers for common read/write patterns, but the byte
  stream remains the underlying truth.

## Typed IO Is Opt-In

Templated `read<T>` and `write<T>` APIs should only participate when `T`
explicitly models a dirtyNet IO concept. If a type does not satisfy the concept,
callers should use the optimized byte-buffer API and wrap application behavior
around it themselves.

This keeps dirtyNet from guessing how arbitrary C++ objects should appear on the
wire.

The typed IO concept should eventually answer questions such as:

- how `T` exposes bytes for writing
- how `T` determines the amount of data needed for reading
- whether `T` is fixed-size, framed, or incrementally decoded
- how malformed application data is reported
- how socket errors, EOF, partial data, and decode failures remain distinct

Compile-time branching can support optimized paths:

- buffer-like values can use direct byte IO
- fixed-size or trivially encoded values can take short paths when explicitly
  opted in
- application-defined encodable/decodable values can call the type's API
- unsupported values should fail at compile time with a clear concept error

The object or adapter that satisfies the concept should own construction and
decode rules. dirtyNet should provide the transport and efficient byte movement,
then call the application-defined API at the boundary.

## Optional Sample Packet Helpers

It may be useful to provide a lightweight templated sample packet or framed
message type as a convenience utility.

Purpose:

- give users a quick way to get started
- demonstrate a recommended framing shape
- offer optimized header and payload encode/decode paths
- serve as a test and example implementation of the typed IO concept

Non-goals:

- do not make this the primary dirtyNet abstraction
- do not require applications to use this packet shape
- do not make dirtyNet own application-level packet semantics

If added, the helper should use explicit encoded sizes and offsets rather than
raw C++ object layout. It should also model payload presence deliberately:
header-only messages and payload-bearing messages are different application
cases, not accidental string/vector states.
