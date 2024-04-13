float afGetVirtualFrameSize (AFfilehandle file, int trackid, int stretch3to4)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return _af_format_frame_size(&track->v, stretch3to4);
}