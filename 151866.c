bool red_stream_set_auto_flush(RedStream *s, bool auto_flush)
{
    if (s->priv->use_cork == !auto_flush) {
        return true;
    }

    s->priv->use_cork = !auto_flush;
    if (s->priv->use_cork) {
        if (socket_set_cork(s->socket, 1)) {
            s->priv->use_cork = false;
            return false;
        } else {
            s->priv->corked = true;
        }
    } else if (s->priv->corked) {
        socket_set_cork(s->socket, 0);
        s->priv->corked = false;
    }
    return true;
}