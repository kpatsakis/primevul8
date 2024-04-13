int afSetVirtualPCMMapping (AFfilehandle file, int trackid,
	double slope, double intercept, double minClip, double maxClip)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	track->v.pcm.slope = slope;
	track->v.pcm.intercept = intercept;
	track->v.pcm.minClip = minClip;
	track->v.pcm.maxClip = maxClip;

	track->ms->setDirty();

	return 0;
}