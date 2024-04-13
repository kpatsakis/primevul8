check_compression(char *path, URLFile *uf)
{
    int len;
    struct compression_decoder *d;

    if (path == NULL)
	return;

    len = strlen(path);
    uf->compression = CMP_NOCOMPRESS;
    for (d = compression_decoders; d->type != CMP_NOCOMPRESS; d++) {
	int elen;
	if (d->ext == NULL)
	    continue;
	elen = strlen(d->ext);
	if (len > elen && strcasecmp(&path[len - elen], d->ext) == 0) {
	    uf->compression = d->type;
	    uf->guess_type = d->mime_type;
	    break;
	}
    }
}