void afGetPCMMapping (AFfilehandle file, int trackid,
	double *slope, double *intercept, double *minClip, double *maxClip)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (slope)
		*slope = track->f.pcm.slope;
	if (intercept)
		*intercept = track->f.pcm.intercept;
	if (minClip)
		*minClip = track->f.pcm.minClip;
	if (maxClip)
		*maxClip = track->f.pcm.maxClip;
}