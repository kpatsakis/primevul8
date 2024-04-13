static int check_duplicate_event(ASS_Track *track, int ReadOrder)
{
    if (track->parser_priv->read_order_bitmap)
        return test_and_set_read_order_bit(track, ReadOrder) > 0;
    // ignoring last event, it is the one we are comparing with
    for (int i = 0; i < track->n_events - 1; i++)
        if (track->events[i].ReadOrder == ReadOrder)
            return 1;
    return 0;
}