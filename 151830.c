RedStreamSslStatus red_stream_enable_ssl(RedStream *stream, SSL_CTX *ctx)
{
    BIO *sbio;

    // Handle SSL handshaking
    if (!(sbio = BIO_new_socket(stream->socket, BIO_NOCLOSE))) {
        spice_warning("could not allocate ssl bio socket");
        return RED_STREAM_SSL_STATUS_ERROR;
    }

    stream->priv->ssl = SSL_new(ctx);
    if (!stream->priv->ssl) {
        spice_warning("could not allocate ssl context");
        BIO_free(sbio);
        return RED_STREAM_SSL_STATUS_ERROR;
    }

    SSL_set_bio(stream->priv->ssl, sbio, sbio);

    stream->priv->write = stream_ssl_write_cb;
    stream->priv->read = stream_ssl_read_cb;
    red_stream_disable_writev(stream);

    return red_stream_ssl_accept(stream);
}