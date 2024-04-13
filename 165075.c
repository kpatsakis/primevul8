void afInitPCMMapping (AFfilesetup setup, int trackid,
	double slope, double intercept, double minClip, double maxClip)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	track->f.pcm.slope = slope;
	track->f.pcm.intercept = intercept;
	track->f.pcm.minClip = minClip;
	track->f.pcm.maxClip = maxClip;
}