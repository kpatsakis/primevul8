static void async_read_handler(G_GNUC_UNUSED int fd,
                               G_GNUC_UNUSED int event,
                               RedStream *stream)
{
    AsyncRead *async = &stream->priv->async_read;
    SpiceCoreInterfaceInternal *core = stream->priv->core;

    for (;;) {
        int n = async->end - async->now;

        spice_assert(n > 0);
        n = red_stream_read(stream, async->now, n);
        if (n <= 0) {
            int err = n < 0 ? errno: 0;
            switch (err) {
            case EAGAIN:
                if (!stream->watch) {
                    stream->watch = core->watch_new(stream->socket,
                                                    SPICE_WATCH_EVENT_READ,
                                                    async_read_handler, stream);
                }
                return;
            case EINTR:
                break;
            default:
                async_read_clear_handlers(stream);
                if (async->error) {
                    async->error(async->opaque, err);
                }
                return;
            }
        } else {
            async->now += n;
            if (async->now == async->end) {
                async_read_clear_handlers(stream);
                async->done(async->opaque);
                return;
            }
        }
    }
}