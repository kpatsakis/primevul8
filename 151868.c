static ssize_t stream_read_cb(RedStream *s, void *buf, size_t size)
{
    return socket_read(s->socket, buf, size);
}