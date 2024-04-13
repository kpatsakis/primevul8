static void red_stream_set_socket(RedStream *stream, int socket)
{
    stream->socket = socket;
    /* deprecated fields. Filling them for backward compatibility */
    stream->priv->info->llen = sizeof(stream->priv->info->laddr);
    stream->priv->info->plen = sizeof(stream->priv->info->paddr);
    getsockname(stream->socket, (struct sockaddr*)(&stream->priv->info->laddr), &stream->priv->info->llen);
    getpeername(stream->socket, (struct sockaddr*)(&stream->priv->info->paddr), &stream->priv->info->plen);

    stream->priv->info->flags |= SPICE_CHANNEL_EVENT_FLAG_ADDR_EXT;
    stream->priv->info->llen_ext = sizeof(stream->priv->info->laddr_ext);
    stream->priv->info->plen_ext = sizeof(stream->priv->info->paddr_ext);
    getsockname(stream->socket, (struct sockaddr*)(&stream->priv->info->laddr_ext),
                &stream->priv->info->llen_ext);
    getpeername(stream->socket, (struct sockaddr*)(&stream->priv->info->paddr_ext),
                &stream->priv->info->plen_ext);
}