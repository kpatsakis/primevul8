static ssize_t red_stream_sasl_read(RedStream *s, uint8_t *buf, size_t nbyte)
{
    uint8_t encoded[4096];
    const char *decoded;
    unsigned int decodedlen;
    int err;
    int n, offset;

    offset = spice_buffer_copy(&s->priv->sasl.inbuffer, buf, nbyte);
    if (offset > 0) {
        spice_buffer_remove(&s->priv->sasl.inbuffer, offset);
        if (offset == nbyte)
            return offset;
        nbyte -= offset;
        buf += offset;
    }

    n = s->priv->read(s, encoded, sizeof(encoded));
    if (n <= 0) {
        return offset > 0 ? offset : n;
    }

    err = sasl_decode(s->priv->sasl.conn,
                      (char *)encoded, n,
                      &decoded, &decodedlen);
    if (err != SASL_OK) {
        spice_warning("sasl_decode error: %d", err);
        errno = EIO;
        return offset > 0 ? offset : -1;
    }

    if (decodedlen == 0) {
        errno = EAGAIN;
        return offset > 0 ? offset : -1;
    }

    n = MIN(nbyte, decodedlen);
    memcpy(buf, decoded, n);
    spice_buffer_append(&s->priv->sasl.inbuffer, decoded + n, decodedlen - n);
    return offset + n;
}