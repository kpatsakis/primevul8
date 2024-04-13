void red_stream_push_channel_event(RedStream *s, int event)
{
    RedsState *reds = s->priv->reds;
    MainDispatcher *md = reds_get_main_dispatcher(reds);
    md->channel_event(event, s->priv->info);
}