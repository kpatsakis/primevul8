int ass_track_set_feature(ASS_Track *track, ASS_Feature feature, int enable)
{
    switch (feature) {
    case ASS_FEATURE_INCOMPATIBLE_EXTENSIONS:
        //-fallthrough
#ifdef USE_FRIBIDI_EX_API
    case ASS_FEATURE_BIDI_BRACKETS:
        track->parser_priv->bidi_brackets = !!enable;
#endif
        return 0;
    default:
        return -1;
    }
}