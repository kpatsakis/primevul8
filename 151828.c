static ssize_t stream_websocket_read(RedStream *s, void *buf, size_t size)
{
    unsigned flags;
    int len;

    do {
        len = websocket_read(s->priv->ws, (uint8_t *) buf, size, &flags);
    } while (len == 0 && flags != 0);
    return len;
}