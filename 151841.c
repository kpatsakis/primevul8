void red_stream_set_async_error_handler(RedStream *stream,
                                        AsyncReadError error_handler)
{
    stream->priv->async_read.error = error_handler;
}