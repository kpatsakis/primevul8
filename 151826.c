static ssize_t stream_websocket_writev(RedStream *s, const struct iovec *iov, int iovcnt)
{
    return websocket_writev(s->priv->ws, (struct iovec *) iov, iovcnt, WEBSOCKET_BINARY_FINAL);
}