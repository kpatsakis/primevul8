int afSetVirtualChannels (AFfilehandle file, int trackid, int channelCount)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	track->v.channelCount = channelCount;
	track->ms->setDirty();

	if (track->channelMatrix)
		free(track->channelMatrix);
	track->channelMatrix = NULL;

	return 0;
}