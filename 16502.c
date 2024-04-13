static int fake_filldir(void *buf, const char *name, int namelen,
			loff_t offset, u64 ino, unsigned d_type)
{
	return 0;
}