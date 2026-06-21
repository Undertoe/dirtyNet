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