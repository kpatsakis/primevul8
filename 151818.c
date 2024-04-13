ssize_t red_stream_writev(RedStream *s, const struct iovec *iov, int iovcnt)
{
    int i;
    int n;
    ssize_t ret = 0;

    if (s->priv->writev != NULL && iovcnt > 1) {
        return s->priv->writev(s, iov, iovcnt);
    }

    for (i = 0; i < iovcnt; ++i) {
        n = red_stream_write(s, iov[i].iov_base, iov[i].iov_len);
        if (n <= 0)
            return ret == 0 ? n : ret;
        ret += n;
    }

    return ret;
}