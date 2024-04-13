void ass_process_chunk(ASS_Track *track, char *data, int size,
                       long long timecode, long long duration)
{
    char *str = NULL;
    int eid;
    char *p;
    char *token;
    ASS_Event *event;
    int check_readorder = track->parser_priv->check_readorder;

    if (check_readorder && !track->parser_priv->read_order_bitmap) {
        for (int i = 0; i < track->n_events; i++) {
            if (test_and_set_read_order_bit(track, track->events[i].ReadOrder) < 0)
                break;
        }
    }

    if (!track->event_format) {
        ass_msg(track->library, MSGL_WARN, "Event format header missing");
        goto cleanup;
    }

    str = malloc(size + 1);
    if (!str)
        goto cleanup;
    memcpy(str, data, size);
    str[size] = '\0';
    ass_msg(track->library, MSGL_V, "Event at %" PRId64 ", +%" PRId64 ": %s",
           (int64_t) timecode, (int64_t) duration, str);

    eid = ass_alloc_event(track);
    if (eid < 0)
        goto cleanup;
    event = track->events + eid;

    p = str;

    do {
        NEXT(p, token);
        event->ReadOrder = atoi(token);
        if (check_readorder && check_duplicate_event(track, event->ReadOrder))
            break;

        NEXT(p, token);
        event->Layer = atoi(token);

        process_event_tail(track, event, p, 3);

        event->Start = timecode;
        event->Duration = duration;

        goto cleanup;
//              dump_events(tid);
    } while (0);
    // some error
    ass_free_event(track, eid);
    track->n_events--;

cleanup:
    free(str);
}