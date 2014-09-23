entangle-server
====

Concurrent File Editor Server

Overview
----

This is the backend to the `entangle` concurrent file editor. The server implements a corrected dOPT algorithm (Cormack 1995) to deal with concurrent editing.

Protocol
----

Commands sent to the server is of the form `CMD:AUX`, where `CMD` is a four-letter command, and `AUX` is the auxiliary information.

* `JOIN:PORT:HOST` -- Join a remote server `HOST:PORT`.
* `DROP:PORT:HOST` -- Drop connection to remote server `HOST:PORT`.
* `SYNC:DATA` -- Current context of the server; the client must set its own context to `DATA`.
* `INSE:UPD_T` -- Insert a character at the specified position to remote server context.
* `DELE:UPD_T` -- Delete a character from the server context.
* `SAVE` -- Requires the server to save its context.

The format of `UPD_T` is specified in `OTNode::enc_upd_t` and `OTNode::dec_upd_t`.

Resources
----

1. [*A Counterexample to the Distributed Operational Transform and a Corrected Algorithm for Point-to-point 
Communication*](https://cs.uwaterloo.ca/research/tr/1995/08/dopt.pdf)
2. [Thoughts and Stuff](http://blog.blogzhang.com/categories/dopt/)
