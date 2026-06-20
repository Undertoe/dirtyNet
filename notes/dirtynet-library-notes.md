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
