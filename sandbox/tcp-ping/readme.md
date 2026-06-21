# TCP Blocking Echo Server

## Shape

- One server listens on `127.0.0.1:port`.
- One client connects, sends bytes, receives echoed bytes.
- Server handles one connection at a time.

## What It Teaches

- TCP has an explicit listen/accept/connect lifecycle.
- TCP is a byte stream, not a message protocol.
- `send` and `recv` may transfer fewer bytes than requested.
- EOF and connection reset are real states to handle.

## Good Follow-Up Tasks

- Send multiple logical messages in one connection.
- Split one logical message across multiple sends.
- Add a length-prefixed message protocol.
- Observe behavior when client closes normally vs exits abruptly.

## Interview Angle

- Explain why TCP does not preserve application message boundaries.
- Explain partial reads/writes.
- Explain graceful close vs reset.
