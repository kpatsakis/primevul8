static ssize_t stream_websocket_write(RedStream *s, const void *buf, size_t size)
{
    return websocket_write(s->priv->ws, buf, size, WEBSOCKET_BINARY_FINAL);
}