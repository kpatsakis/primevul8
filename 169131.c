static ASS_Track *parse_memory(ASS_Library *library, char *buf)
{
    ASS_Track *track;
    int i;

    track = ass_new_track(library);
    if (!track)
        return NULL;

    // process header
    process_text(track, buf);

    // external SSA/ASS subs does not have ReadOrder field
    for (i = 0; i < track->n_events; ++i)
        track->events[i].ReadOrder = i;

    if (track->track_type == TRACK_TYPE_UNKNOWN) {
        ass_free_track(track);
        return 0;
    }

    ass_process_force_style(track);

    return track;
}