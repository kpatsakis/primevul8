ssize_t red_stream_write(RedStream *s, const void *buf, size_t nbyte)
{
    ssize_t ret;

#if HAVE_SASL
    if (s->priv->sasl.conn && s->priv->sasl.runSSF) {
        ret = red_stream_sasl_write(s, buf, nbyte);
    } else
#endif
        ret = s->priv->write(s, buf, nbyte);

    return ret;
}