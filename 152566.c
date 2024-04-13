content_set_byte_order (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, entry_set_byte_order, data);
}