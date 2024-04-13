void ass_free_track(ASS_Track *track)
{
    int i;

    if (!track)
        return;

    if (track->parser_priv) {
        free(track->parser_priv->read_order_bitmap);
        free(track->parser_priv->fontname);
        free(track->parser_priv->fontdata);
        free(track->parser_priv);
    }
    free(track->style_format);
    free(track->event_format);
    free(track->Language);
    if (track->styles) {
        for (i = 0; i < track->n_styles; ++i)
            ass_free_style(track, i);
    }
    free(track->styles);
    if (track->events) {
        for (i = 0; i < track->n_events; ++i)
            ass_free_event(track, i);
    }
    free(track->events);
    free(track->name);
    free(track);
}