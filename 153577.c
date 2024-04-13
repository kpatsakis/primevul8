clear_ignore_p_flag(int cmd, struct readbuffer *obuf)
{
    static int clear_flag_cmd[] = {
	HTML_HR, HTML_UNKNOWN
    };
    int i;

    for (i = 0; clear_flag_cmd[i] != HTML_UNKNOWN; i++) {
	if (cmd == clear_flag_cmd[i]) {
	    obuf->flag &= ~RB_IGNORE_P;
	    return;
	}
    }
}