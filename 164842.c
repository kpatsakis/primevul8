_Track *_af_filehandle_get_track (AFfilehandle file, int trackid)
{
	int	i;
	for (i=0; i<file->trackCount; i++)
	{
		if (file->tracks[i].id == trackid)
			return &file->tracks[i];
	}

	_af_error(AF_BAD_TRACKID, "bad track id %d", trackid);

	return NULL;
}