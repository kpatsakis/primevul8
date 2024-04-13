_PUBLIC_ bool file_save(const char *fname, const void *packet, size_t length)
{
	return file_save_mode(fname, packet, length, 0644);
}