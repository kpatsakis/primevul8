static inline void async_read_clear_handlers(RedStream *stream)
{
    AsyncRead *async = &stream->priv->async_read;
    red_stream_remove_watch(stream);
    async->now = NULL;
    async->end = NULL;
}