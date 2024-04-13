int afSetTrackPCMMapping (AFfilehandle file, int trackid,
	double slope, double intercept, double minClip, double maxClip)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	/*
		NOTE: this is highly unusual: we don't ordinarily
		change track.f after the file is opened.

		PCM mapping is the exception because this information
		is not encoded in sound files' headers using today's
		formats, so the user will probably want to set this
		information on a regular basis.  The defaults may or
		may not be what the user wants.
	*/

	track->f.pcm.slope = slope;
	track->f.pcm.intercept = intercept;
	track->f.pcm.minClip = minClip;
	track->f.pcm.maxClip = maxClip;

	track->ms->setDirty();

	return 0;
}