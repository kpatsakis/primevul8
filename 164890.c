void afGetVirtualSampleFormat (AFfilehandle file, int trackid, int *sampleFormat, int *sampleWidth)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (sampleFormat != NULL)
		*sampleFormat = track->v.sampleFormat;

	if (sampleFormat != NULL)
		*sampleWidth = track->v.sampleWidth;
}