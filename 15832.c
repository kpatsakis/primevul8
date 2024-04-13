emacs_tags_incstack_free(void)
{
    while (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = NULL;
	VIM_CLEAR(incstack[incstack_idx].etag_fname);
    }
}