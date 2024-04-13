int red_stream_get_family(const RedStream *s)
{
    spice_return_val_if_fail(s != NULL, -1);

    if (s->socket == -1)
        return -1;

    return s->priv->info->laddr_ext.ss_family;
}