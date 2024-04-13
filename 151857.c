static ssize_t stream_ssl_error(RedStream *s, int return_code)
{
    SPICE_GNUC_UNUSED int ssl_error;

    ssl_error = SSL_get_error(s->priv->ssl, return_code);

    // OpenSSL can to return SSL_ERROR_WANT_READ if we attempt to read
    // data and the socket did not receive all SSL packet.
    // Under Windows errno is not set so potentially caller can detect
    // the wrong error so we need to set errno.
#ifdef _WIN32
    if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
        errno = EAGAIN;
    } else {
        errno = EPIPE;
    }
#endif

    // red_peer_receive is expected to receive -1 on errors while
    // OpenSSL documentation just state a <0 value
    return -1;
}