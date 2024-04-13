static bool detect_legacy_conv_subs(ASS_Track *track)
{
    /*
     * FFmpeg and libav convert srt subtitles to ass.
     * In legacy versions, they did not set the 'ScaledBorderAndShadow' header,
     * but expected it to default to yes (which libass did).
     * To avoid breaking them, we try to detect these
     * converted subs by common properties of ffmpeg/libav's converted subs.
     * Since files with custom format lines (-2014.10.11) default to SBAS=1
     * regardless of being ffmpeg generated or not, we are only concerned with
     * post-signature and pre-SBAS ffmpeg-files (2014.10.11-2020.04.17).
     * We want to avoid matching modified ffmpeg files though.
     *
     * Relevant ffmpeg commits are:
     *  2c77c90684e24ef16f7e7c4462e011434cee6a98  2010.12.29
     *    Initial conversion format.
     *    Style "Format:" line is mix of SSA and ASS
     *    Event "Format:" line
     *     "Format: Layer, Start, End, Text\r\n"
     *    Only Header in ScriptInfo is "ScriptType: v4.00+"
     *  0e7782c08ec77739edb0b98ba5d896b45e98235f  2012.06.15
     *    Adds 'Style' to Event "Format:" line
     *  5039aadf68deb9ad6dd0737ea11259fe53d3727b  2014.06.18
     *    Adds PlayerRes(X|Y) (384x288)
     *    (moved below ScriptType: a few minutes later)
     *  40b9f28641b696c6bb73ce49dc97c2ce2700cbdb  2014.10.11 14:31:23 +0200
     *    Regular full ASS Event and Style "Format:" lines
     *  52b0a0ecaa02e17f7e01bead8c3f215f1cfd48dc  2014.10.11 18:37:43 +0200 <==
     *    Signature comment
     *  56bc0a6736cdc7edab837ff8f304661fd16de0e4  2015.02.08
     *    Allow custom PlayRes(X|Y)
     *  a8ba2a2c1294a330a0e79ae7f0d3a203a7599166  2020.04.17
     *    Set 'ScaledBorderAndShadow: yes'
     *
     * libav outputs initial ffmpeg format. (no longer maintained)
     */

    // GENBY_FFMPEG and exact ffmpeg headers required
    // Note: If there's SINFO_SCRIPTTYPE in the future this needs to be updated
    if (track->parser_priv->header_flags
            ^ (SINFO_PLAYRESX | SINFO_PLAYRESY | GENBY_FFMPEG))
        return false;

    // Legacy ffmpeg only ever has one style
    // Check 2 not 1 because libass also adds a def style
    if (track->n_styles != 2
        || strncmp(track->styles[1].Name, "Default", 7))
        return false;

    return true;
}