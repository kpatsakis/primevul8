compress_application_type(int compression)
{
    struct compression_decoder *d;

    for (d = compression_decoders; d->type != CMP_NOCOMPRESS; d++) {
	if (d->type == compression)
	    return d->mime_type;
    }
    return NULL;
}