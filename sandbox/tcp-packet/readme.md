# TCP Framed Protocol Example

## Shape

- Build a tiny binary or text framing layer over TCP.
- Suggested first version: 4-byte length prefix plus payload.
- Client sends several frames; server parses and echoes complete frames.

## What It Teaches

- Demonstrate that TCP carries bytes, not application packets.
- TCP is a stream and needs application framing.
- Input buffers must handle partial frames and multiple frames per read.
- Protocol parsing often drives buffer design.

## Parser Hints

Think of the parser as a small state machine that only inspects the bytes
currently available at the front of an incoming buffer.

The parser should not read from the socket and should not own the receive
buffer. Its job is to answer one question:

- Can exactly one complete packet be decoded from the bytes available now?

Suggested state movement:

1. Need header
   - If there are not enough bytes for the fixed header, report incomplete.
   - Otherwise decode the packet type and decide what the packet needs next.
2. Classify packet type
   - Unknown packet types are invalid.
   - Header-only packets, such as ping or pong, are complete after the header.
   - Payload-bearing packets continue to length parsing.
3. Need length
   - If there are not enough bytes for the payload length field, report
     incomplete.
   - Otherwise decode the payload length and calculate the total packet size.
4. Need payload
   - If the full payload is not available yet, report incomplete.
   - Otherwise build the complete packet.
5. Complete
   - Return the decoded packet.
   - Return how many bytes were consumed.

The caller owns accumulation and consumption:

- Append newly read socket bytes to an incoming buffer.
- Keep parsing complete packets from the front of that buffer.
- Remove only the bytes consumed by each complete packet.
- Stop and keep the remaining bytes when the parser reports incomplete.
- Treat invalid input as a protocol error.

Not enough bytes is a normal TCP parser state, not a socket error.

## Good Follow-Up Tasks

- Enforce max frame size.
- Handle malformed frames.
- Add simple request ids.

## Interview Angle

- Explain how application protocols sit above TCP.
- Explain why networking libraries often need buffer abstractions.


## CURRENT STATUS: 
- Refactoring encoding to run off of a byte stream & ptrs
- refactor decoding to have a staged decoding off of ptrs: 
  - parse a header, then branch based on what we get
  - update the ptr in the buffer 
  - application logic should have a loop to parse packets for now
