end_visual_mode_keep_button()
{
#ifdef FEAT_CLIPBOARD
    /*
     * If we are using the clipboard, then remember what was selected in case
     * we need to paste it somewhere while we still own the selection.
     * Only do this when the clipboard is already owned.  Don't want to grab
     * the selection when hitting ESC.
     */
    if (clip_star.available && clip_star.owned)
	clip_auto_select();

# if defined(FEAT_EVAL)
    // Emit a TextYankPost for the automatic copy of the selection into the
    // star and/or plus register.
    if (has_textyankpost())
    {
	if (clip_isautosel_star())
	    call_yank_do_autocmd('*');
	if (clip_isautosel_plus())
	    call_yank_do_autocmd('+');
    }
# endif
#endif

    VIsual_active = FALSE;
    setmouse();
    mouse_dragging = 0;

    // Save the current VIsual area for '< and '> marks, and "gv"
    curbuf->b_visual.vi_mode = VIsual_mode;
    curbuf->b_visual.vi_start = VIsual;
    curbuf->b_visual.vi_end = curwin->w_cursor;
    curbuf->b_visual.vi_curswant = curwin->w_curswant;
#ifdef FEAT_EVAL
    curbuf->b_visual_mode_eval = VIsual_mode;
#endif
    if (!virtual_active())
	curwin->w_cursor.coladd = 0;
    may_clear_cmdline();

    adjust_cursor_eol();
}