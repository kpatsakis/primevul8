static long rdg_bio_ctrl(BIO* bio, int cmd, long arg1, void* arg2)
{
	long status = -1;
	rdpRdg* rdg = (rdpRdg*)BIO_get_data(bio);
	rdpTls* tlsOut = rdg->tlsOut;
	rdpTls* tlsIn = rdg->tlsIn;

	if (cmd == BIO_CTRL_FLUSH)
	{
		(void)BIO_flush(tlsOut->bio);
		(void)BIO_flush(tlsIn->bio);
		status = 1;
	}
	else if (cmd == BIO_C_SET_NONBLOCK)
	{
		status = 1;
	}
	else if (cmd == BIO_C_READ_BLOCKED)
	{
		BIO* bio = tlsOut->bio;
		status = BIO_read_blocked(bio);
	}
	else if (cmd == BIO_C_WRITE_BLOCKED)
	{
		BIO* bio = tlsIn->bio;
		status = BIO_write_blocked(bio);
	}
	else if (cmd == BIO_C_WAIT_READ)
	{
		int timeout = (int)arg1;
		BIO* bio = tlsOut->bio;

		if (BIO_read_blocked(bio))
			return BIO_wait_read(bio, timeout);
		else if (BIO_write_blocked(bio))
			return BIO_wait_write(bio, timeout);
		else
			status = 1;
	}
	else if (cmd == BIO_C_WAIT_WRITE)
	{
		int timeout = (int)arg1;
		BIO* bio = tlsIn->bio;

		if (BIO_write_blocked(bio))
			status = BIO_wait_write(bio, timeout);
		else if (BIO_read_blocked(bio))
			status = BIO_wait_read(bio, timeout);
		else
			status = 1;
	}
	else if (cmd == BIO_C_GET_EVENT || cmd == BIO_C_GET_FD)
	{
		/*
		 * A note about BIO_C_GET_FD:
		 * Even if two FDs are part of RDG, only one FD can be returned here.
		 *
		 * In FreeRDP, BIO FDs are only used for polling, so it is safe to use the outgoing FD only
		 *
		 * See issue #3602
		 */
		status = BIO_ctrl(tlsOut->bio, cmd, arg1, arg2);
	}

	return status;
}