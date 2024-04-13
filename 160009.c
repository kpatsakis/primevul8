void create_mapping(struct file_mapping& data, const std::string& fn)
{
#ifdef LIBRAW_WIN32_CALLS
	std::wstring fpath(fn.begin(), fn.end());
	if ((data.fd = CreateFileW(fpath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE) return;
	LARGE_INTEGER fs;
	if (!GetFileSizeEx(data.fd, &fs)) return;
	data.fsize = fs.QuadPart;
	if ((data.fd_map = ::CreateFileMapping(data.fd, 0, PAGE_READONLY, fs.HighPart, fs.LowPart, 0)) == INVALID_HANDLE_VALUE) return;
	data.map = MapViewOfFile(data.fd_map, FILE_MAP_READ, 0, 0, data.fsize);
#else
	struct stat stt;
	if ((data.fd = open(fn.c_str(), O_RDONLY)) < 0) return;
	if (fstat(data.fd, &stt) != 0) return;
	data.fsize = stt.st_size;
	data.map = mmap(0, data.fsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, data.fd, 0);
	return;
#endif
}