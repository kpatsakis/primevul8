bool red_stream_is_websocket(RedStream *stream, const void *buf, size_t len)
{
    if (stream->priv->ws) {
        return false;
    }

    stream->priv->ws = websocket_new(buf, len, stream,
                                     (websocket_read_cb_t) stream->priv->read,
                                     (websocket_write_cb_t) stream->priv->write,
                                     (websocket_writev_cb_t) stream->priv->writev);
    if (stream->priv->ws) {
        stream->priv->read = stream_websocket_read;
        stream->priv->write = stream_websocket_write;

        if (stream->priv->writev) {
            stream->priv->writev = stream_websocket_writev;
        }

        return true;
    }

    return false;
}