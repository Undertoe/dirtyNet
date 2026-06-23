# TCP Packet Sandbox Last Steps

Use this sandbox to demonstrate TCP stream behavior clearly before extracting
any dirtyNet library abstractions.

## Goal

Show that TCP is a byte stream, not a packet stream.

The example should prove that the server can correctly decode:

- multiple application packets received from one read buffer
- one application packet split across multiple reads
- a packet payload larger than the server's temporary read buffer
- header-only packets and payload-bearing packets in the same stream

## Shared Packet/Decode Work

Add a stream-aware decode helper instead of assuming one `read` equals one
packet.

Recommended shape:

- keep `packet` as the representation of one complete decoded packet
- add a decode result that reports complete, incomplete, or invalid input
- return how many bytes were consumed when a full packet is decoded
- treat missing header bytes as incomplete
- treat missing payload bytes as incomplete
- treat illegal packet type and payload combinations as invalid

The decoder should only construct a complete packet after enough bytes exist for
both the header and the declared payload length.

Keep the decoding model deliberately simple:

1. Decode the fixed-size header first.
2. Use the information from that header to decide how much remaining data must
   be decoded for the complete packet.

This should be the main mental model for the exercise. Do not try to decode a
whole packet at once from whatever bytes happened to arrive in one socket read.
The header tells the application how to interpret the remaining bytes.

## Server Work

Change the server read loop to accumulate bytes across reads.

Flow:

1. Read into a small temporary buffer.
2. If `read` returns `0`, treat it as peer disconnect and exit cleanly.
3. If `read` returns a negative value, report the socket error and exit cleanly.
4. Append bytes read into a persistent incoming byte buffer.
5. Repeatedly try to decode packets from the front of the incoming buffer.
6. Stop decoding when the buffer only contains an incomplete packet.
7. Remove consumed bytes after each complete packet.
8. Treat invalid packet data as an application decode error.

Use a deliberately small temporary read buffer, such as 64 or 128 bytes, so the
oversized-payload case is easy to observe.

## Client Work

Change the client to build several encoded packets and send them as one stream.

Suggested packet sequence:

- `ping`
- small greeting message
- large greeting message, larger than the server read buffer
- `pong` or another header-only packet
- `QUIT`

The client can concatenate all encoded packets into one byte vector and call a
write-all helper once. This should demonstrate that the server cannot assume
that one read maps to one application packet.

## Expected Output

The server should log each decoded application packet separately even when the
bytes arrive through arbitrary TCP reads.

Useful debug output:

- number of bytes returned by each socket read
- number of bytes currently accumulated
- decoded packet type
- decoded payload length
- number of bytes consumed by each decoded packet
- clean exit on `QUIT` or peer disconnect

## Library Lesson

This exercise should feed the dirtyNet API direction without becoming the
library's core protocol.

Takeaway:

- socket APIs move bytes
- TCP receive loops must handle EOF, errors, partial data, and accumulated data
- application-level framing belongs above the socket layer
- typed dirtyNet IO should be opt-in through concepts or adapters
- sample packet helpers can exist as examples, not as the primary abstraction
