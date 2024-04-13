ins_ctrl_hat(void)
{
    if (map_to_exists_mode((char_u *)"", LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	if (State & LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~LANGMAP;
	}
	else
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= LANGMAP;
#ifdef HAVE_INPUT_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_INPUT_METHOD
    else
    {
	// There are no ":lmap" mappings, toggle IM
	if (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	else
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~LANGMAP;
	    im_set_active(TRUE);
	}
    }
#endif
    set_iminsert_global();
    showmode();
#ifdef FEAT_GUI
    // may show different cursor shape or color
    if (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#if defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}