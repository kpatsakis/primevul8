void ass_process_force_style(ASS_Track *track)
{
    char **fs, *eq, *dt, *style, *tname, *token;
    ASS_Style *target;
    int sid;
    char **list = track->library->style_overrides;

    if (!list)
        return;

    for (fs = list; *fs; ++fs) {
        eq = strrchr(*fs, '=');
        if (!eq)
            continue;
        *eq = '\0';
        token = eq + 1;

        if (!ass_strcasecmp(*fs, "PlayResX"))
            track->PlayResX = atoi(token);
        else if (!ass_strcasecmp(*fs, "PlayResY"))
            track->PlayResY = atoi(token);
        else if (!ass_strcasecmp(*fs, "Timer"))
            track->Timer = ass_atof(token);
        else if (!ass_strcasecmp(*fs, "WrapStyle"))
            track->WrapStyle = atoi(token);
        else if (!ass_strcasecmp(*fs, "ScaledBorderAndShadow"))
            track->ScaledBorderAndShadow = parse_bool(token);
        else if (!ass_strcasecmp(*fs, "Kerning"))
            track->Kerning = parse_bool(token);
        else if (!ass_strcasecmp(*fs, "YCbCr Matrix"))
            track->YCbCrMatrix = parse_ycbcr_matrix(token);

        dt = strrchr(*fs, '.');
        if (dt) {
            *dt = '\0';
            style = *fs;
            tname = dt + 1;
        } else {
            style = NULL;
            tname = *fs;
        }
        for (sid = 0; sid < track->n_styles; ++sid) {
            if (style == NULL
                || ass_strcasecmp(track->styles[sid].Name, style) == 0) {
                target = track->styles + sid;
                PARSE_START
                    STRVAL(FontName)
                    COLORVAL(PrimaryColour)
                    COLORVAL(SecondaryColour)
                    COLORVAL(OutlineColour)
                    COLORVAL(BackColour)
                    FPVAL(FontSize)
                    INTVAL(Bold)
                    INTVAL(Italic)
                    INTVAL(Underline)
                    INTVAL(StrikeOut)
                    FPVAL(Spacing)
                    FPVAL(Angle)
                    INTVAL(BorderStyle)
                    INTVAL(Alignment)
                    INTVAL(Justify)
                    INTVAL(MarginL)
                    INTVAL(MarginR)
                    INTVAL(MarginV)
                    INTVAL(Encoding)
                    FPVAL(ScaleX)
                    FPVAL(ScaleY)
                    FPVAL(Outline)
                    FPVAL(Shadow)
                    FPVAL(Blur)
                PARSE_END
            }
        }
        *eq = '=';
        if (dt)
            *dt = '.';
    }
}