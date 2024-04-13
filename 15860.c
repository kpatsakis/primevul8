free_tag_stuff(void)
{
    ga_clear_strings(&tag_fnames);
    if (curwin != NULL)
	do_tag(NULL, DT_FREE, 0, 0, 0);
    tag_freematch();

# if defined(FEAT_QUICKFIX)
    tagstack_clear_entry(&ptag_entry);
# endif
}