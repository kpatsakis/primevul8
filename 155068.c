win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_TEXT_PROP
    win_T	*wp;

    for (wp = first_popupwin; wp != NULL; wp = wp->w_next)
	if (wp == win)
	    return TRUE;
    for (wp = curtab->tp_first_popupwin; wp != NULL; wp = wp->w_next)
	if (wp == win)
	    return TRUE;
#endif
    return FALSE;
}