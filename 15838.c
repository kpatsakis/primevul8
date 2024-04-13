tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    int		c;

#ifdef FEAT_EMACS_TAGS
    if (tagp->is_etag)
	c = 0;	    // to shut up GCC
    else
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    if (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    return fullname;
}