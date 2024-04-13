display_showcmd(void)
{
    int	    len;

    cursor_off();

    len = (int)STRLEN(showcmd_buf);
    if (len == 0)
	showcmd_is_clear = TRUE;
    else
    {
	screen_puts(showcmd_buf, (int)Rows - 1, sc_col, 0);
	showcmd_is_clear = FALSE;
    }

    /*
     * clear the rest of an old message by outputting up to SHOWCMD_COLS
     * spaces
     */
    screen_puts((char_u *)"          " + len, (int)Rows - 1, sc_col + len, 0);

    setcursor();	    // put cursor back where it belongs
}