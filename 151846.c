bool red_stream_is_ssl(RedStream *stream)
{
    return (stream->priv->ssl != NULL);
}