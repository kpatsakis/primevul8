int afSetVirtualSampleFormat (AFfilehandle file, int trackid,
	int sampleFormat, int sampleWidth)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (_af_set_sample_format(&track->v, sampleFormat, sampleWidth) == AF_FAIL)
		return -1;

	track->ms->setDirty();

	return 0;
}