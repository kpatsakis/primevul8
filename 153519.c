convertLine0(URLFile *uf, Str line, int mode)
#endif
{
#ifdef USE_M17N
    line = wc_Str_conv_with_detect(line, charset, doc_charset, InnerCharset);
#endif
    if (mode != RAW_MODE)
	cleanup_line(line, mode);
#ifdef USE_NNTP
    if (uf && uf->scheme == SCM_NEWS)
	Strchop(line);
#endif				/* USE_NNTP */
    return line;
}