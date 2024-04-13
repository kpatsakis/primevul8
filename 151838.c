bool red_stream_is_plain_unix(const RedStream *s)
{
    spice_return_val_if_fail(s != NULL, false);

    if (red_stream_get_family(s) != AF_UNIX) {
        return false;
    }

#if HAVE_SASL
    if (s->priv->sasl.conn) {
        return false;
    }
#endif
    if (s->priv->ssl) {
        return false;
    }

    return true;

}