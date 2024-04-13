void ass_process_codec_private(ASS_Track *track, char *data, int size)
{
    ass_process_data(track, data, size);

    // probably an mkv produced by ancient mkvtoolnix
    // such files don't have [Events] and Format: headers
    if (!track->event_format)
        event_format_fallback(track);

    ass_process_force_style(track);
}