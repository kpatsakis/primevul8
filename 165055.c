void afGetSampleFormat (AFfilehandle file, int trackid, int *sampleFormat, int *sampleWidth)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (sampleFormat != NULL)
		*sampleFormat = track->f.sampleFormat;

	if (sampleFormat != NULL)
		*sampleWidth = track->f.sampleWidth;
}