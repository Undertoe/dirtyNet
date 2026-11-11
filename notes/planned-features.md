# Planned Features

Use this file for reusable dirtyNet feature directions that are not yet stable
public API. Detailed implementation order belongs in `notes/working-todos.md`.

## Guiding Boundary

dirtyNet is a generic networking layer. It should make sockets easier to own,
compose, and operate efficiently without embedding an application protocol or
forcing callers into one execution or storage model.

The blocking byte APIs remain the dependable lower layer. Typed operations,
execution strategies, and storage policies should be optional composition over
that layer.

## API Demand Tracking

New use cases should be written first as caller demands: the operation needed,
ownership and lifetime constraints, error behavior, composition needs, and any
measurable performance pressure. Only then should they become API proposals.

Prefer one narrow public addition that can be implemented and exercised end to
end. Keep related convenience overloads, policy systems, and speculative
generality as separate follow-up candidates. This keeps reviews understandable
and makes it possible to attribute correctness and performance changes to a
specific API decision.

## IPv4 Multicast Binding

Provide explicit factories that return an ordinary `udp_socket` already bound
and joined to a group.

Initial direction:

- accept a multicast group endpoint;
- optionally accept a local IPv4 interface address;
- bind the wildcard local address at the group port;
- join one any-source group;
- preserve existing UDP send and receive operations;
- make membership lifetime follow native socket ownership.

Do not add a public interface-enumeration abstraction until concrete needs make
an IP address insufficient.

Later multicast extensions may include multiple memberships, explicit leave,
IPv6, source-specific multicast, and sender controls.

## Blocking TCP Connection

Provide a distinct move-only outbound stream resource rather than a generic
public socket base.

Initial direction:

- connect using an endpoint;
- read into caller-owned storage;
- write from caller-owned storage;
- expose partial progress through byte counts;
- distinguish orderly EOF from errors;
- support close and minimal shutdown.

Stream framing belongs to the caller. `read_exactly`, `write_all`, or framed
helpers can later compose over the partial-I/O primitives.

Listeners and accepted connections are separate later features because they
introduce different ownership and execution questions.

## Hostname Resolution

Keep resolution separate from literal IP parsing and endpoint value semantics.
Resolution should return multiple endpoint candidates rather than hiding
selection and connection fallback inside a single opaque call.

Questions to resolve with implementation evidence:

- How are candidate ordering and address-family filters represented?
- Does service-name lookup belong in the first resolver or only numeric ports?
- Which portable error categories retain enough native diagnostic value?

## External Consumption And Packaging

The library must be usable by an independent CMake project through a stable
namespaced target. Begin with the smallest source-based consumption path, then
add install and package-export support when it solves a demonstrated need.

The consumer proof should validate that include paths, C++ standard
requirements, and future transitive dependencies propagate through the target.

## Execution Strategies

Future high-level use should be able to coordinate repeated operations and
multiple sockets without every application rebuilding the same machinery.

Relevant design pressure includes:

- single-threaded event loops;
- dedicated receive workers;
- caller-provided executors or schedulers;
- batching and backpressure;
- cancellation, shutdown, and terminal results;
- timers and retry coordination.

Do not equate this feature with simply putting a thread inside every socket.
Socket ownership, operation scheduling, and callback execution are separate
decisions that need explicit contracts.

## Storage And Data Adaptation

Direct I/O should support caller-owned byte storage so high-throughput callers
can control allocation and lifetime. Later options may include reusable buffers,
fixed pools, rings, batches, and allocator-aware storage.

Typed send/receive facilities should be opt-in adapters over the byte layer.
They must define serialization, framing, ownership, and validation explicitly;
the library should not reinterpret arbitrary object memory as a wire format.

## Measurement Support

Performance is a project requirement, not an unsupported claim. Benchmarks
should make changes comparable by recording workload, payload size, build mode,
machine context, repetitions, throughput, and relevant resource costs.

Correctness and API work should not silently degrade throughput. A measured
regression must be investigated and either corrected or deliberately accepted
with its tradeoff documented.

## Portability And Protocol Growth

Likely later work includes:

- IPv6 completion across all established seams;
- a Windows backend behind the same public contracts;
- TCP listeners and accepted connections;
- connected UDP where it provides a concrete benefit;
- portable error refinement informed by more than one protocol;
- protocol-neutral batching and readiness integration.

Compile-time typing remains an option where it prevents invalid combinations,
but it should not make ordinary UDP or TCP use substantially noisier. Distinct
resource types are the current baseline; templates should be introduced only
when repeated implementations reveal a useful generic dimension.
