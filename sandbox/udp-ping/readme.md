# UDP 1-to-1 Ping/Pong

## Shape

- One UDP server binds to `127.0.0.1:port`.
- One UDP client sends a small message with `sendto`.
- Server receives with `recvfrom`, prints sender address, and replies.
- Client receives reply and prints round-trip time.

## What It Teaches

- UDP is datagram-oriented, not stream-oriented.
- `recvfrom` gives both payload and peer address.
- There is no connection handshake.
- Message boundaries are preserved up to the receive buffer size.

## Good Follow-Up Tasks

- Send messages larger than the receive buffer and observe truncation behavior.
- Add sequence numbers and timestamps.
- Add timeout handling with `select`, `poll`, or socket receive timeout.
- Compare `sendto`/`recvfrom` with `connect` on a UDP socket.

## Interview Angle

- Explain TCP streams vs UDP datagrams.
- Explain what reliability UDP does not provide.
- Explain why market data often uses UDP/multicast-like patterns.
