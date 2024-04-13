void ass_flush_events(ASS_Track *track)
{
    if (track->events) {
        int eid;
        for (eid = 0; eid < track->n_events; eid++)
            ass_free_event(track, eid);
        track->n_events = 0;
    }
    free(track->parser_priv->read_order_bitmap);
    track->parser_priv->read_order_bitmap = NULL;
    track->parser_priv->read_order_elems = 0;
}