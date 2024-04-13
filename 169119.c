void ass_free_style(ASS_Track *track, int sid)
{
    ASS_Style *style = track->styles + sid;

    free(style->Name);
    free(style->FontName);
}