static char *red_stream_get_local_address(RedStream *stream)
{
    return addr_to_string("%s;%s", &stream->priv->info->laddr_ext,
                          stream->priv->info->llen_ext);
}