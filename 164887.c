int afGetVirtualChannels (AFfilehandle file, int trackid)
{
	_Track	*track;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->v.channelCount;
}