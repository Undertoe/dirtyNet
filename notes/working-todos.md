# Working TODOs

This is the high-level coding checklist for the current dirtyNet implementation
pass. It records what needs to be built without fixing exact API signatures,
parameter lists, result types, or factory names.

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

## After The Address Pass

- Review implementation evidence and refine the portable error model.
- Add native socket ownership.
- Add blocking UDP factory functions.
- Add `send_to`, direct `receive`, and one-shot `on_receive` under the UDP
  socket object.
- Add outbound packet-concept support through `to_bytes` for socket send/write
  operations.
- Validate blocking UDP behavior with ping/pong.
- Add explicit hostname resolution returning multiple endpoint candidates.

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
