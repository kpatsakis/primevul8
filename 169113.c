ASS_Track *ass_new_track(ASS_Library *library)
{
    int def_sid = -1;
    ASS_Track *track = calloc(1, sizeof(ASS_Track));
    if (!track)
        goto fail;
    track->library = library;
    track->ScaledBorderAndShadow = 0;
    track->parser_priv = calloc(1, sizeof(ASS_ParserPriv));
    if (!track->parser_priv)
        goto fail;
    def_sid = ass_alloc_style(track);
    if (def_sid < 0)
        goto fail;
    set_default_style(track->styles + def_sid);
    track->default_style = def_sid;
    if (!track->styles[def_sid].Name || !track->styles[def_sid].FontName)
        goto fail;
    track->parser_priv->check_readorder = 1;
    return track;

fail:
    if (track) {
        if (def_sid >= 0)
            ass_free_style(track, def_sid);
        free(track->parser_priv);
        free(track);
    }
    return NULL;
}