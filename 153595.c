acceptableEncoding()
{
    static Str encodings = NULL;
    struct compression_decoder *d;
    TextList *l;
    char *p;

    if (encodings != NULL)
	return encodings->ptr;
    l = newTextList();
    for (d = compression_decoders; d->type != CMP_NOCOMPRESS; d++) {
	if (check_command(d->cmd, d->auxbin_p)) {
	    pushText(l, d->encoding);
	}
    }
    encodings = Strnew();
    while ((p = popText(l)) != NULL) {
	if (encodings->length)
	    Strcat_charp(encodings, ", ");
	Strcat_charp(encodings, p);
    }
    return encodings->ptr;
}