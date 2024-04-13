int ass_alloc_style(ASS_Track *track)
{
    int sid;

    assert(track->n_styles <= track->max_styles);

    if (track->n_styles == track->max_styles) {
        if (track->max_styles >= FFMIN(SIZE_MAX, INT_MAX) - ASS_STYLES_ALLOC)
            return -1;
        int new_max = track->max_styles + ASS_STYLES_ALLOC;
        if (!ASS_REALLOC_ARRAY(track->styles, new_max))
            return -1;
        track->max_styles = new_max;
    }

    sid = track->n_styles++;
    memset(track->styles + sid, 0, sizeof(ASS_Style));
    return sid;
}