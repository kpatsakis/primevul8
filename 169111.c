static void event_format_fallback(ASS_Track *track)
{
    track->parser_priv->state = PST_EVENTS;
    if (track->track_type == TRACK_TYPE_SSA)
        track->event_format = strdup(ssa_event_format);
    else
        track->event_format = strdup(ass_event_format);
    ass_msg(track->library, MSGL_V,
            "No event format found, using fallback");
}