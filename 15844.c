expand_tags(
    int		tagnames,	// expand tag names
    char_u	*pat,
    int		*num_file,
    char_u	***file)
{
    int		i;
    int		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    int		ret;

    name_buf = alloc(name_buf_size);
    if (name_buf == NULL)
	return FAIL;

    if (tagnames)
	extra_flag = TAG_NAMES;
    else
	extra_flag = 0;
    if (pat[0] == '/')
	ret = find_tags(pat + 1, num_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NO_TAGFUNC,
		TAG_MANY, curbuf->b_ffname);
    else
	ret = find_tags(pat, num_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NO_TAGFUNC | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    if (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 for (i = 0; i < *num_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     if (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 if (buf == NULL)
		 {
		     vim_free(name_buf);
		     return FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != NULL && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    return ret;
}