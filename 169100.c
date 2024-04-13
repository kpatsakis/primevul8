int ass_alloc_event(ASS_Track *track)
{
    int eid;

    assert(track->n_events <= track->max_events);

    if (track->n_events == track->max_events) {
        if (track->max_events >= FFMIN(SIZE_MAX, INT_MAX) / 2)
            return -1;
        int new_max = track->max_events * 2 + 1;
        if (!ASS_REALLOC_ARRAY(track->events, new_max))
            return -1;
        track->max_events = new_max;
    }

    eid = track->n_events++;
    memset(track->events + eid, 0, sizeof(ASS_Event));
    return eid;
}