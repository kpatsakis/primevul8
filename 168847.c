DWORD rdg_get_event_handles(rdpRdg* rdg, HANDLE* events, DWORD count)
{
	DWORD nCount = 0;
	assert(rdg != NULL);

	if (rdg->tlsOut && rdg->tlsOut->bio)
	{
		if (events && (nCount < count))
		{
			BIO_get_event(rdg->tlsOut->bio, &events[nCount]);
			nCount++;
		}
		else
			return 0;
	}

	if (rdg->tlsIn && rdg->tlsIn->bio)
	{
		if (events && (nCount < count))
		{
			BIO_get_event(rdg->tlsIn->bio, &events[nCount]);
			nCount++;
		}
		else
			return 0;
	}

	return nCount;
}