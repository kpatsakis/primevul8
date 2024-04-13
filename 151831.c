static ssize_t stream_ssl_write_cb(RedStream *s, const void *buf, size_t size)
{
    int return_code;

    return_code = SSL_write(s->priv->ssl, buf, size);

    if (return_code < 0) {
        return stream_ssl_error(s, return_code);
    }

    return return_code;
}