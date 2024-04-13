win_init_empty(win_T *wp)
{
    redraw_win_later(wp, NOT_VALID);
    wp->w_lines_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.col = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	/* pcmark not cleared but set to line 1 */
    wp->w_pcmark.col = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.col = 0;
    wp->w_topline = 1;
#ifdef FEAT_DIFF
    wp->w_topfill = 0;
#endif
    wp->w_botline = 2;
#if defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_buffer->b_s;
#endif
}