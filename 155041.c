check_split_disallowed()
{
    if (split_disallowed > 0)
    {
	emsg(_("E242: Can't split a window while closing another"));
	return FAIL;
    }
    return OK;
}