static int rdg_bio_read(BIO* bio, char* buf, int size)
{
	int status;
	rdpRdg* rdg = (rdpRdg*)BIO_get_data(bio);
	status = rdg_read_data_packet(rdg, (BYTE*)buf, size);

	if (status < 0)
	{
		BIO_clear_retry_flags(bio);
		return -1;
	}
	else if (status == 0)
	{
		BIO_set_retry_read(bio);
		WSASetLastError(WSAEWOULDBLOCK);
		return -1;
	}
	else
	{
		BIO_set_flags(bio, BIO_FLAGS_READ);
	}

	return status;
}