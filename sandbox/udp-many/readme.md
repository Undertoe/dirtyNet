# UDP 1-to-Many Broadcast-Style Local Fanout

## Shape

- One UDP sender periodically emits numbered messages.
- Multiple receiver processes bind locally and print what they receive.
- Start with localhost/unicast to separate ports.
- Optionally extend later to broadcast or multicast after the basic behavior is clear.

## What It Teaches

- UDP supports fanout patterns better than TCP's connection model.
- Receivers can miss messages without the sender knowing.
- Application-level sequence numbers matter.
- Loss, reordering, and duplicate handling belong above UDP.

## Good Follow-Up Tasks

- Add sequence-gap detection on receivers.
- Add a receiver that intentionally sleeps to simulate falling behind.
- Track per-receiver observed gaps.
- Add a simple snapshot request path over TCP later, mirroring market-data recovery patterns.

## Interview Angle

- Explain why market data systems care about sequence numbers.
- Explain snapshot plus incremental update recovery.
- Explain why UDP can be attractive when timeliness matters more than guaranteed delivery.
