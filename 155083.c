win_split(int size, int flags)
{
    if (ERROR_IF_POPUP_WINDOW)
	return FAIL;

    /* When the ":tab" modifier was used open a new tab page instead. */
    if (may_open_tabpage() == OK)
	return OK;

    /* Add flags from ":vertical", ":topleft" and ":botright". */
    flags |= cmdmod.split;
    if ((flags & WSP_TOP) && (flags & WSP_BOT))
    {
	emsg(_("E442: Can't split topleft and botright at the same time"));
	return FAIL;
    }
    if (check_split_disallowed() == FAIL)
	return FAIL;

    /* When creating the help window make a snapshot of the window layout.
     * Otherwise clear the snapshot, it's now invalid. */
    if (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    else
	clear_snapshot(curtab, SNAP_HELP_IDX);

    return win_split_ins(size, flags, NULL, 0);
}