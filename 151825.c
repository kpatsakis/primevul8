void red_stream_set_channel(RedStream *stream, int connection_id,
                            int channel_type, int channel_id)
{
    stream->priv->info->connection_id = connection_id;
    stream->priv->info->type = channel_type;
    stream->priv->info->id   = channel_id;
    if (red_stream_is_ssl(stream)) {
        stream->priv->info->flags |= SPICE_CHANNEL_EVENT_FLAG_TLS;
    }
}