entangle-server protocol
====

Format
----

A message packet to / from `entangle-server` is expected to have the following format (where each field is delimited by `:`):

| `L` | `ACK` | `MSG_ID` | `CLIENT_ID` | `CMD` | `ERR` | `AUX` |

### Example

```
# client-side connection request
19::182::CONN::foobar123
```

Where:

* `L` -- The total length of the packet, starting from the `ACK` field and ending at the last character of `AUX`. This does **not** include the `:` delimiter between `L` 
	and `ACK`.
* `ACK` -- Set to `0` or left empty on a *REQUEST* and set to `1` on a *RESPONSE*.
* `MSG_ID` -- Unique *numeric* message id for each *REQUEST*; this is mirrored on the *RESPONSE*.
* `CLIENT_ID` -- Unique client id for a client; this is left empty on the original connection request and is filled in by the server.
* `CMD` -- A *word* designating the command.
* `ERR` -- A numeric code designating an error.
* `AUX` -- Additional data for each command.

Connection
----

Disconnect
----

Resize
----

Diff
----

Seek
----

Overwrite
----

Insert
----

Erase
----

Error Codes
----

| Code | Designation | description |
| ---- | ----------- | ----------- |
| 0001 | ACC_DENIED  | access denied |
| 0002 | MAX_CONN    | max connections |
