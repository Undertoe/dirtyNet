# TCP Multi-Client Server: One Thread Per Connection

## Shape

- The server accepts multiple clients.
- Each accepted socket is handled by a dedicated thread.
- The protocol stays simple: echo or line-based messages.

## Exercise Goals

- Implement the socket behavior in the scaffolded client and server entry points.
- Keep blocking I/O and thread ownership easy to follow.
- Avoid adding event loops or reusable dirtyNet abstractions in this exercise.

## What It Teaches

- Blocking I/O can be simple but scales poorly.
- Thread-per-connection creates scheduling and memory overhead.
- Shared state requires synchronization.

## Good Follow-Up Tasks

- Track connected client count safely.
- Add a broadcast-to-all-clients command.
- Measure rough behavior with many idle clients.

## Interview Angle

- Explain context switching and scheduler pressure.
- Explain why event loops are commonly used for many connections.

## Planned Implementation

### Protocol

- Use a newline-delimited text protocol.
- Each greeting is one request and ends with `\n`.
- Each response contains the reversed greeting and ends with `\n`.
- Treat TCP as a byte stream: a read may contain part of one message, one
  complete message, or several messages.
- Keep a receive buffer for each connection, extract every complete line, and
  preserve any incomplete trailing bytes for the next read.
- Use a send loop because one call to `send()` may write only part of a message.

### Server Flow

1. Create, bind, and listen on the server socket.
2. Keep the main server thread in an accept loop.
3. For each accepted connection, start a dedicated worker thread that owns the
   connected socket and its receive buffer.
4. In the worker, repeatedly read bytes and append them to the receive buffer.
5. For every complete newline-delimited greeting, reverse its text and send the
   newline-delimited result back to the client.
6. Continue until `read()` returns `0`, indicating that the client has closed
   its sending side.
7. Close the connected socket and end that worker thread.

Data read from an accepted socket belongs only to that TCP connection. Other
clients have their own accepted sockets, worker threads, and receive buffers.

### Client Flow

1. Connect to the server using the port supplied on the command line.
2. Select a series of greetings from a small set.
3. Send every newline-delimited greeting before reading any responses:

   ```text
   send(greeting[0])
   send(greeting[1])
   ...
   send(greeting[n])
   ```

4. Read and accumulate response bytes until the number of complete responses
   matches the number of greetings sent.
5. Output each response as it is decoded and verify that it is the reversed form
   of the corresponding greeting.
6. Close the connection after every expected response has been received.

The client counts complete newline-delimited responses, not calls to `read()`.
The server processes each connection's greetings sequentially, so that client's
responses should arrive in the same order as its requests.

### Error Behavior

- A fatal setup error involving the listening socket terminates the server.
- A read or send error on one accepted connection is reported and terminates
  only that connection's worker; the server continues serving other clients.
- A client-side connect, send, or read error is reported and terminates that
  client application.
- If either side receives EOF before completing its expected protocol work, it
  reports the premature disconnect.
