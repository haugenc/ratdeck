# LXMF Wire Format Reference

This documents the exact LXMF wire format as implemented by Python Sideband/NomadNet and ratdeck. All fields are big-endian.

## Canonical (Internal) Format

Used for signing, messageId computation, and storage:

```
[dest_hash:16][src_hash:16][signature:64][msgpack_payload]
```

- `dest_hash` — 16-byte truncated hash of the destination identity
- `src_hash` — 16-byte truncated hash of the source identity
- `signature` — 64-byte Ed25519 signature
- `msgpack_payload` — MsgPack array: `[timestamp, title, content, fields]`

## Wire Formats by Delivery Method

### Opportunistic (single-packet, non-link)

The destination hash is **not** included in the LXMF payload — it's carried by the RNS packet header.

```
RNS Packet payload = [src_hash:16][signature:64][msgpack_payload]
```

Python reference (`LXMessage.py:628-631`):
```python
if self.method == LXMessage.OPPORTUNISTIC:
    return RNS.Packet(self.__delivery_destination, self.packed[DESTINATION_LENGTH:])
```

### Direct (link-based)

The destination hash IS included in the payload:

```
Link packet payload = [dest_hash:16][src_hash:16][signature:64][msgpack_payload]
```

Python reference:
```python
elif self.method == LXMessage.DIRECT:
    return RNS.Packet(self.__delivery_destination, self.packed)
```

## Receiving

On receive, the router reconstructs the canonical format before unpacking:

- **Non-link packets**: Prepend `packet.destination.hash` to the data
- **Link packets**: Data already contains dest_hash

Python reference (`LXMRouter.py:1821-1828`):
```python
if packet.destination_type != RNS.Destination.LINK:
    lxmf_data = packet.destination.hash + data   # prepend dest_hash
else:
    lxmf_data = data                              # already has dest_hash
```

## Signature Computation

The signature covers the canonical data **plus** a message hash:

```
hashed_part = dest_hash || src_hash || msgpack_payload
msg_hash    = SHA256(hashed_part)
signable    = hashed_part || msg_hash
signature   = Ed25519_sign(signable)
```

## Message ID

```
messageId = SHA256(dest_hash || src_hash || msgpack_payload)
```

This is the same as `msg_hash` above — computed from the hashed_part before appending the hash for signing. It must be identical on sender and receiver for deduplication.

## MsgPack Payload

Fixed 4-element array (`0x94`):

| Index | Field     | MsgPack Type | Notes |
|-------|-----------|-------------|-------|
| 0     | timestamp | float64 (0xCB) | Unix epoch seconds |
| 1     | title     | bin8/bin16 (0xC4/0xC5) | Python expects bytes, not str |
| 2     | content   | bin8/bin16 (0xC4/0xC5) | Python expects bytes, not str |
| 3     | fields    | fixmap (0x80) | Empty map for basic messages |
