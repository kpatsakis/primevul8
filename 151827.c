void red_stream_async_read(RedStream *stream,
                           uint8_t *data, size_t size,
                           AsyncReadDone read_done_cb,
                           void *opaque)
{
    AsyncRead *async = &stream->priv->async_read;

    g_return_if_fail(async->now == NULL && async->end == NULL);
    if (size == 0) {
        read_done_cb(opaque);
        return;
    }
    async->now = data;
    async->end = async->now + size;
    async->done = read_done_cb;
    async->opaque = opaque;
    async_read_handler(0, 0, stream);

}