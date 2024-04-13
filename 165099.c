int afSetVirtualRate (AFfilehandle file, int trackid, double rate)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (rate < 0)
	{
		_af_error(AF_BAD_RATE, "invalid sampling rate %.30g", rate);
		return -1;
	}

	track->v.sampleRate = rate;
	track->ms->setDirty();

	return 0;
}