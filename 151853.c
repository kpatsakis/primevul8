static ssize_t stream_ssl_read_cb(RedStream *s, void *buf, size_t size)
{
    int return_code;

    return_code = SSL_read(s->priv->ssl, buf, size);

    if (return_code < 0) {
        return stream_ssl_error(s, return_code);
    }

    return return_code;
}