entry_set_byte_order (ExifEntry *e, void *data)
{
	ByteOrderChangeData *d = data;

	if (!e)
		return;

	exif_array_set_byte_order (e->format, e->data, e->components, d->old, d->new);
}