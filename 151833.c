void red_stream_remove_watch(RedStream* s)
{
    red_watch_remove(s->watch);
    s->watch = NULL;
}