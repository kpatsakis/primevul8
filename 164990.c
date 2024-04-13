ssize_t FileModule::write(const void *data, size_t nbytes)
{
	return m_fh->write(data, nbytes);
}