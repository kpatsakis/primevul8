ssize_t FileModule::read(void *data, size_t nbytes)
{
	return m_fh->read(data, nbytes);
}