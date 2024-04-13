found_tagfile_cb(char_u *fname, void *cookie UNUSED)
{
    if (ga_grow(&tag_fnames, 1) == OK)
    {
	char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(tag_fname);
#endif
	simplify_filename(tag_fname);
	((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
    }
}