static void red_stream_disable_writev(RedStream *stream)
{
    stream->priv->writev = NULL;
}