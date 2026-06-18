# dirtyNet
 Clean and dirty Network Library


not needed but nice to have: https://github.com/Undertoe/.devcontainer.git for running the project within a dev environment.   


# REQUIRED LIBRARIES: 
Catch2 - https://github.com/catchorg/Catch2


# Primary Objects: 
Server - 
1) A 1 -> many connection interface.  Listening will allow the user to bind an "on read" function.
2) Provides a write to a specific connection OR connection lookup (key, typically ip/port combo)
3) Connections are always handled asynchronously
4) Connection steps are also implemented by the user through the ConnectionStep interface.  
    - variadic template passed to a variant.  Each templated type is required to implement the ConnectionStep requirements

Writer - 
1) Single use connection type that will allow you to write to a destination.
2) These can be set up as persistent or single fire.
3) Async or Sync

Reader - 
1) Setup to listen to a destination for data.
2) Setup to be persistent or single use.
3) Async or Sync

General Purpose Socket
1) Setup to read & write to a connection
2) persistent only
3) async or sync


GOAL: HTTP Socket
1) implementation of the "General Purpose socket" to handle basic HTTP Requests.
2) Initial methods: get / put / post 

all objects are bound to a specific protocol, tcp or udp.  
    - Compile time distinction?



Below this, all connections are implemented over a "socket" which is handling the udp vs tcp connection information.