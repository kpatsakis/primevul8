_TrackSetup *_af_filesetup_get_tracksetup (AFfilesetup setup, int trackid)
{
	int	i;
	for (i=0; i<setup->trackCount; i++)
	{
		if (setup->tracks[i].id == trackid)
			return &setup->tracks[i];
	}

	_af_error(AF_BAD_TRACKID, "bad track id %d", trackid);

	return NULL;
}