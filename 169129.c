static int process_events_line(ASS_Track *track, char *str)
{
    if (!strncmp(str, "Format:", 7)) {
        char *p = str + 7;
        skip_spaces(&p);
        free(track->event_format);
        track->event_format = strdup(p);
        if (!track->event_format)
            return -1;
        ass_msg(track->library, MSGL_DBG2, "Event format: %s", track->event_format);
        if (track->track_type == TRACK_TYPE_ASS)
            custom_format_line_compatibility(track, p, ass_event_format);
        else
            custom_format_line_compatibility(track, p, ssa_event_format);

        // Guess if we are dealing with legacy ffmpeg subs and change accordingly
        // If file has no event format it was probably not created by ffmpeg/libav
        if (detect_legacy_conv_subs(track)) {
            track->ScaledBorderAndShadow = 1;
            ass_msg(track->library, MSGL_INFO,
                    "Track treated as legacy ffmpeg sub.");
        }
    } else if (!strncmp(str, "Dialogue:", 9)) {
        // This should never be reached for embedded subtitles.
        // They have slightly different format and are parsed in ass_process_chunk,
        // called directly from demuxer
        int eid;
        ASS_Event *event;

        // We can't parse events without event_format
        if (!track->event_format) {
            event_format_fallback(track);
            if (!track->event_format)
                return -1;
        }

        str += 9;
        skip_spaces(&str);

        eid = ass_alloc_event(track);
        if (eid < 0)
            return -1;
        event = track->events + eid;

        return process_event_tail(track, event, str, 0);
    } else {
        ass_msg(track->library, MSGL_V, "Not understood: '%.30s'", str);
    }
    return 0;
}