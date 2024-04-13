static ssize_t stream_write_cb(RedStream *s, const void *buf, size_t size)
{
    return socket_write(s->socket, buf, size);
}