add_tag_field(
    dict_T  *dict,
    char    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    int		len = 0;
    int		retval;

    // check that the field name doesn't exist yet
    if (dict_has_key(dict, field_name))
    {
	if (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	return FAIL;
    }
    buf = alloc(MAXPATHL);
    if (buf == NULL)
	return FAIL;
    if (start != NULL)
    {
	if (end == NULL)
	{
	    end = start + STRLEN(start);
	    while (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (int)(end - start);
	if (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    return retval;
}