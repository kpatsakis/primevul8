_TrackSetup *_af_tracksetup_new (int trackCount)
{
	int		i;
	_TrackSetup	*tracks;

	if (trackCount == 0)
		return NULL;

	tracks = _af_calloc(trackCount, sizeof (_TrackSetup));
	if (tracks == NULL)
		return NULL;

	for (i=0; i<trackCount; i++)
	{
		tracks[i] = _af_default_tracksetup;

		tracks[i].id = AF_DEFAULT_TRACK + i;

		/* XXXmpruett deal with compression */

		_af_set_sample_format(&tracks[i].f, tracks[i].f.sampleFormat,
			tracks[i].f.sampleWidth);

		if (tracks[i].markerCount == 0)
			tracks[i].markers = NULL;
		else
		{
			int	j;
			tracks[i].markers = _af_calloc(tracks[i].markerCount,
				sizeof (_MarkerSetup));

			if (tracks[i].markers == NULL)
				return NULL;

			for (j=0; j<tracks[i].markerCount; j++)
			{
				tracks[i].markers[j].id = j+1;

				tracks[i].markers[j].name = _af_strdup("");
				if (tracks[i].markers[j].name == NULL)
					return NULL;

				tracks[i].markers[j].comment = _af_strdup("");
				if (tracks[i].markers[j].comment == NULL)
					return NULL;
			}
		}
	}

	return tracks;
}