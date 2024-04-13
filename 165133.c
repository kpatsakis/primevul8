AFfileoffset afGetTrackBytes (AFfilehandle file, int trackid)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	track = _af_filehandle_get_track(file, trackid);

	return track->data_size;
}