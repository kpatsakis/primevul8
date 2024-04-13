void red_stream_set_core_interface(RedStream *stream, SpiceCoreInterfaceInternal *core)
{
    red_stream_remove_watch(stream);
    stream->priv->core = core;
}