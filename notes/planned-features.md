# Planned Features

Use this file for reusable dirtyNet feature ideas before they are ready to
become source code. During the experimental phase, keep entries loosely scoped:
capture direction and design pressure without turning notes into a binding API
spec too early.

## Address Foundation

Goal: reduce cognitive load around socket address setup, byte order, and
address-family details before building higher-level socket abstractions.

These pieces may start as separate value types, but the main user-facing shape
will likely be an endpoint object that combines an IP address and port.

### Port Object Abstraction

Provide a small value type for network ports.

Intent:

- Store port values in host byte order internally.
- Convert to network byte order at the POSIX boundary with `htons`.
- Convert from network byte order with `ntohs` when reading from socket address
  structures.
- Keep port `0` valid for "let the OS choose an ephemeral port."
- Make raw byte-order handling rare in application-facing code.

Open questions:

- Should construction accept only `std::uint16_t`, or should there be checked
  factories for wider integer and string inputs?
- Should the type be named `port`, `network_port`, or something protocol scoped?
- Should service-name lookup be handled elsewhere, or stay out of the first
  version?

### IP Object Abstraction

Provide an IP address value type that can represent both IPv4 and IPv6.

Intent:

- Hide the difference between IPv4 and IPv6 storage from most caller code.
- Preserve the address family explicitly so endpoint and socket setup can choose
  the correct POSIX structures.
- Support parsing from text and formatting back to text.
- Keep conversion to `sockaddr_in`, `sockaddr_in6`, or `sockaddr_storage`
  centralized instead of repeated across examples and library code.
- Leave room for loopback, any-address, and possibly multicast helpers without
  forcing them into raw socket setup code.

Open questions:

- Should this be named `ip_address`, `address`, or split into `ipv4_address` and
  `ipv6_address` with a wrapper type?
- Should DNS/name resolution be a separate feature from literal IP address
  parsing?
- Should endpoint construction own address-family compatibility checks between
  `ip_address` and `port`?

### Endpoint Object Abstraction

Provide a complete endpoint value type made from an IP address and a port.

Intent:

- Represent "where a socket binds or sends" as one object.
- Keep IP-family, port, and socket-address conversion decisions together.
- Let socket APIs consume endpoints instead of separate address and port values.
- Reduce repeated `sockaddr` setup across experiments without hiding the core
  socket behavior too aggressively.

Loose first shape:

- `endpoint = ip_address + port`
- usable for local bind targets and remote send/connect targets
- convertible at the POSIX boundary into the appropriate socket-address storage

Open questions:

- Should endpoint be generic across IPv4 and IPv6 from the start, or should
  experiments begin with IPv4 and leave the shape ready for IPv6?
- Should local endpoints and remote endpoints be the same type?
- How much POSIX storage should endpoint own directly versus produce on demand?

## Typed Socket Direction

The long-term direction is for socket type objects to be typed through
templates. The exact model is intentionally undecided.

Possible design pressure:

- Protocol distinctions may become compile-time types, such as UDP vs TCP.
- Address-family distinctions may also be represented in the type system, or
  carried by runtime address values.
- The template model should reduce invalid combinations without making basic
  socket usage noisy.
- The first API should stay small enough that the raw Linux socket behavior is
  still easy to recognize.

Open questions:

- Should socket templates be parameterized by protocol, address family, role, or
  some smaller set of traits?
- Should UDP and TCP have separate concrete socket types first, with templates
  introduced only after the repeated shape is obvious?
- How much compile-time typing is helpful before it starts increasing the
  learning cost?
