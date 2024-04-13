test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    while ((p = vim_strchr(p, '\t')) != NULL)
    {
	++p;
	if (STRNCMP(p, "file:", 5) == 0)
	    return TRUE;
    }

    return FALSE;
}