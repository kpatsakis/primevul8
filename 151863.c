void red_stream_flush(RedStream *s)
{
    if (s->priv->corked) {
        socket_set_cork(s->socket, 0);
        socket_set_cork(s->socket, 1);
    }
}