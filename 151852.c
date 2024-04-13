static char *red_stream_get_remote_address(RedStream *stream)
{
    return addr_to_string("%s;%s", &stream->priv->info->paddr_ext,
                          stream->priv->info->plen_ext);
}