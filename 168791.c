static int rdg_bio_write(BIO* bio, const char* buf, int num)
{
	int status;
	rdpRdg* rdg = (rdpRdg*)BIO_get_data(bio);
	BIO_clear_flags(bio, BIO_FLAGS_WRITE);
	EnterCriticalSection(&rdg->writeSection);
	status = rdg_write_data_packet(rdg, (const BYTE*)buf, num);
	LeaveCriticalSection(&rdg->writeSection);

	if (status < 0)
	{
		BIO_clear_flags(bio, BIO_FLAGS_SHOULD_RETRY);
		return -1;
	}
	else if (status < num)
	{
		BIO_set_flags(bio, BIO_FLAGS_WRITE);
		WSASetLastError(WSAEWOULDBLOCK);
	}
	else
	{
		BIO_set_flags(bio, BIO_FLAGS_WRITE);
	}

	return status;
}