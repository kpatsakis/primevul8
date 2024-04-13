uncompressed_file_type(char *path, char **ext)
{
    int len, slen;
    Str fn;
    char *t0;
    struct compression_decoder *d;

    if (path == NULL)
	return NULL;

    slen = 0;
    len = strlen(path);
    for (d = compression_decoders; d->type != CMP_NOCOMPRESS; d++) {
	if (d->ext == NULL)
	    continue;
	slen = strlen(d->ext);
	if (len > slen && strcasecmp(&path[len - slen], d->ext) == 0)
	    break;
    }
    if (d->type == CMP_NOCOMPRESS)
	return NULL;

    fn = Strnew_charp(path);
    Strshrink(fn, slen);
    if (ext)
	*ext = filename_extension(fn->ptr, 0);
    t0 = guessContentType(fn->ptr);
    if (t0 == NULL)
	t0 = "text/plain";
    return t0;
}