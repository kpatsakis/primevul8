void close_mapping(struct file_mapping& data)
{
#ifdef LIBRAW_WIN32_CALLS
	if (data.map) UnmapViewOfFile(data.map);
	if (data.fd_map != INVALID_HANDLE_VALUE) CloseHandle(data.fd_map);
	if (data.fd != INVALID_HANDLE_VALUE) CloseHandle(data.fd);
	data.map = 0;
	data.fsize = 0;
	data.fd = data.fd_map = INVALID_HANDLE_VALUE;
#else
	if (data.map)
		munmap(data.map, data.fsize);
	if (data.fd >= 0)
		close(data.fd);
	data.map = 0;
	data.fsize = 0;
	data.fd = -1;
#endif
}