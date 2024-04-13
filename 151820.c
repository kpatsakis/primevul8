ssize_t red_stream_read(RedStream *s, void *buf, size_t nbyte)
{
    ssize_t ret;

#if HAVE_SASL
    if (s->priv->sasl.conn && s->priv->sasl.runSSF) {
        ret = red_stream_sasl_read(s, (uint8_t*) buf, nbyte);
    } else
#endif
        ret = s->priv->read(s, buf, nbyte);

    return ret;
}