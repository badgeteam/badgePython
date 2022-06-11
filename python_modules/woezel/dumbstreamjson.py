def _from_stream(stream, keys=None, blocksize=512):
    """
    Really dumb implementation of a tokenizer for JSON arrays of objects.
    Yields every item of the root array as a JSON object. Does not work on arrays of other types.
    :param stream: A file-like stream
    :param blocksize: An optional list of keys to extract. If set, items are returned with all entries but these stripped.
    :param keys: The number of bytes to read from the stream at once.
    :return:
    """
    import gc, ujson
    start = stream.read(1)
    start = start.decode() if type(start) is bytes else start
    if start != '[':
        raise ValueError('Content is not a JSON array of objects without unnecessary whitespace')

    buffer = stream.read(blocksize)
    object_counter = 0
    token = bytearray(4096)
    write_pointer = 0
    while buffer:
        gc.collect()
        for char in [b'%c' % char for char in buffer]:
            if write_pointer < 4000:
                token[write_pointer] = ord(char)
                token[write_pointer+1] = 0
            write_pointer += 1
            if char == b'{':
                object_counter += 1
                if object_counter == 1:
                    write_pointer = 1
                    token[0] = ord('{')
            elif char == b'}':
                object_counter -= 1
                if object_counter == 0 and write_pointer < 4000:
                    item = ujson.loads(token[0:write_pointer].decode())
                    if keys is not None:
                        for key in item.keys():
                            if key not in keys:
                                del item[key]
                    print(item)
                    yield item
                    del item
                    gc.collect()
                    print(gc.mem_free())
                    write_pointer = 0
        buffer = stream.read(blocksize)


def from_url(url, keys=None, blocksize=512):
    import urequests, gc
    print(gc.mem_free())
    r = urequests.get(url)
    print(gc.mem_free())
    return _from_stream(r.raw, keys, blocksize)

from_file = _from_stream