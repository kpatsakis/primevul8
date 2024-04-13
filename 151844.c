RedStream *red_stream_new(RedsState *reds, int socket)
{
    RedStream *stream;

    stream = (RedStream*) g_malloc0(sizeof(RedStream) + sizeof(RedStreamPrivate));
    stream->priv = (RedStreamPrivate *)(stream+1);
    stream->priv->info = g_new0(SpiceChannelEventInfo, 1);
    stream->priv->reds = reds;
    stream->priv->core = reds_get_core_interface(reds);
    red_stream_set_socket(stream, socket);

    stream->priv->read = stream_read_cb;
    stream->priv->write = stream_write_cb;
    stream->priv->writev = stream_writev_cb;

    return stream;
}