static ssize_t red_stream_sasl_write(RedStream *s, const void *buf, size_t nbyte)
{
    ssize_t ret;

    if (!s->priv->sasl.encoded) {
        int err;
        err = sasl_encode(s->priv->sasl.conn, (char *)buf, nbyte,
                          (const char **)&s->priv->sasl.encoded,
                          &s->priv->sasl.encodedLength);
        if (err != SASL_OK) {
            spice_warning("sasl_encode error: %d", err);
            errno = EIO;
            return -1;
        }

        if (s->priv->sasl.encodedLength == 0) {
            return 0;
        }

        if (!s->priv->sasl.encoded) {
            spice_warning("sasl_encode didn't return a buffer!");
            return 0;
        }

        s->priv->sasl.encodedOffset = 0;
    }

    ret = s->priv->write(s, s->priv->sasl.encoded + s->priv->sasl.encodedOffset,
                         s->priv->sasl.encodedLength - s->priv->sasl.encodedOffset);

    if (ret <= 0) {
        return ret;
    }

    s->priv->sasl.encodedOffset += ret;
    if (s->priv->sasl.encodedOffset == s->priv->sasl.encodedLength) {
        s->priv->sasl.encoded = NULL;
        s->priv->sasl.encodedOffset = s->priv->sasl.encodedLength = 0;
        return nbyte;
    }

    /* we didn't flush the encoded buffer */
    errno = EAGAIN;
    return -1;
}