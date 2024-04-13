static inline void check_duplicate_info_line(const ASS_Track *const track,
                                             const ScriptInfo si,
                                             const char *const name)
{
    if (track->parser_priv->header_flags & si)
        ass_msg(track->library, MSGL_WARN,
            "Duplicate Script Info Header '%s'. Previous value overwritten!",
            name);
    else
        track->parser_priv->header_flags |= si;
}