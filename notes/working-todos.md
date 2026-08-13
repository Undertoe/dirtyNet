# Working TODOs

This is the high-level coding checklist for the current dirtyNet implementation
pass. It records what needs to be built without fixing exact API signatures,
parameter lists, result types, or factory names.

## Milestone Shape

Implementation is split into two layers:

- **Step 0: UDP proof of concept.** Build the smallest IPv4-only vertical slice
  that can run dirtyNet UDP ping/pong. This proves the address, endpoint, native
  socket, and byte-oriented I/O seams.
- **MVP: exchange-facing foundation.** Extend the proven slice with IPv6,
  hostname resolution, and socket-I/O defaults and options needed to begin
  talking to an exchange. Exact exchange protocol and session behavior remain
  above this networking-core milestone.

Step 0 intentionally favors one concrete, untemplated blocking implementation.
Its purpose is to produce evidence before fixing the broader default and option
system.

## Step 0: Smallest UDP Vertical Slice

### Address Values

- Finish `port` as a host-order public value with native conversion isolated
  behind its access helper.
- Finish numeric `ipv4` parsing, localhost/any creation, formatting, equality,
  and native access.
- Construct the family-erased `ip` value from `ipv4`; IPv4 is the baseline
  family for Step 0.

### Endpoint

- Construct `endpoint` from validated `ip + port` values.
- Provide a fallible numeric-string + port path that parses the address and then
  composes the endpoint.
- Eagerly prepare the POSIX native IPv4 endpoint used by socket calls.
- Convert validated POSIX endpoint data back into public `ip`, `port`, and
  `endpoint` values.
- Add focused public and internal/native round-trip tests. The exact test
  grouping may be refined while the seam is implemented.

### Baseline UDP Socket

- Add move-only native socket ownership and non-throwing cleanup.
- Start with one untemplated public UDP socket type.
- Use a single blocking execution path; do not require the final strategy or
  template system for Step 0.
- Add the minimum bind, `send_to`, and receive operations.
- Return received bytes in an owning byte vector for the baseline path.
- Return or otherwise report the sender endpoint needed to reply to a datagram.
- Keep provisional bind/send/receive errors sufficient for exercising the
  vertical slice.

### Proof

- Build the dirtyNet UDP ping/pong implementation on the Step 0 API.
- Confirm bind, outbound send, inbound bytes, sender endpoint conversion, and
  reply behavior end to end.
- Record which socket-I/O choices should become defaults and which require
  explicit options; do not finalize those policies before the proof runs.

## MVP After Step 0

- Complete `ipv6` and extend `ip`, endpoint conversion, and UDP behavior through
  the already-proven family-neutral seams.
- Add explicit hostname resolution with address-family filtering while
  preserving the resolver behavior selected in the revision 0 design.
- Define the baseline socket-I/O defaults using Step 0 evidence.
- Add explicit options where the baseline exchange workflow needs behavior that
  should not be universal.
- Refine the portable error/result model while retaining useful native
  diagnostic evidence.
- Exercise the resulting networking foundation against the first concrete
  exchange-facing workflow.

## Full Revision 0 Component Checklist

The sections below retain the complete component-level backlog. The Step 0 and
MVP sections above determine implementation order; completion of every item
below is not required before the IPv4 UDP proof runs.

## Project Structure

- Convert `dirtynet` to a header-only CMake `INTERFACE` target.
- Place public headers directly under `lib/dirtynet/`.
- Place shared implementation headers under `lib/dirtynet/detail/`.
- Place the first native backend under `lib/dirtynet/detail/posix/`.
- Use `.hh` for dirtyNet headers.
- Keep maintained executables under `exec/` and automated checks under
  `tests/`.

## `port`

- Store a port value in host byte order.
- Support port `0` for OS-selected ephemeral ports.
- Provide access to the stored numeric value.
- Provide equality and ordinary value-type behavior.
- Handle native byte-order conversion at the native boundary.

## `ipv4`

- Add factory functions for numeric IPv4 parsing.
- Implement `localhost` and `any` creation.
- Implement `string` formatting.
- Store an already-parsed binary IPv4 representation.
- Provide equality and ordinary value-type behavior.
- Provide internal native access without reparsing.

## `ipv6`

- Add factory functions for numeric IPv6 parsing.
- Implement `localhost` and `any` creation.
- Implement `string` formatting.
- Store an already-parsed binary IPv6 representation.
- Provide equality and ordinary value-type behavior.
- Provide internal native access without reparsing.

## `ip`

- Wrap either a valid `ipv4` or `ipv6` value.
- Implement `localhost` and `any` defaults.
- Implement `string` formatting by delegating to the active address family.
- Provide address-family inspection for internal endpoint construction.
- Provide equality across `ip` values while preserving family distinctions.

## `endpoint`

- Construct directly from a valid IP value and `port`.
- Retain the semantic IP and port values.
- Eagerly prepare the complete native endpoint representation.
- Provide access to the semantic address and port.
- Provide internal native access for socket operations.
- Add internal conversion from validated native endpoint data.
- Provide equality and ordinary value-type behavior.

### Next Step: Endpoint Construction

Handle all four construction inputs during revision 0:

- Validated native endpoint data.
- Numeric IP strings.
- Port values, including the intended integer and string forms.
- URL or hostname lookup through a fallible resolution path.

Keep the exact URL/hostname resolution boundary separate from numeric IP
parsing while deciding how its resulting endpoint candidates enter the public
API.

## Native Endpoint Layer

- Define shared `native_endpoint` storage and length handling.
- Implement IPv4 endpoint conversion for POSIX.
- Implement IPv6 endpoint conversion for POSIX.
- Validate address family and native length when converting from OS-provided
  data.
- Convert native ports back to host byte order.
- Keep POSIX address structures out of the ordinary public API.
- Avoid parsing, resolving, allocating, or rebuilding an unchanged endpoint
  during repeated socket operations.

## Provisional Errors And Tests

- Use a small provisional failure/result shape for parsing and native
  conversion.
- Do not treat the first error names or categories as stable API commitments.
- Add focused tests for numeric IPv4 and IPv6 parsing and formatting.
- Add tests for `localhost`, `any`, equality, and invalid numeric input.
- Add tests for port `0` and byte-order conversion.
- Add IPv4 and IPv6 endpoint round-trip tests through the POSIX native layer.

## Later MVP And Post-Proof Work

- Review implementation evidence and refine the portable error model.
- Complete the IPv6 address and native-endpoint path.
- Add hostname resolution after the basic UDP path is proven.
- Add one-shot `on_receive` under the UDP socket object after direct blocking
  receive behavior is established.
- Add outbound packet-concept support through `to_bytes` for socket send/write
  operations.
- Expand socket-I/O defaults and options using the UDP ping/pong evidence.

## Deferred

- Typed receive/read operations.
- Repeated callback operations owned by the library.
- Async execution and callback scheduling.
- TCP resources and stream operations.
- Connected UDP.
- Multicast membership.
- Timeouts and retry policy.
- Windows backend implementation.
- Stable comprehensive error taxonomy.
