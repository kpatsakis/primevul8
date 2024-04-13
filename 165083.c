status _af_filesetup_make_handle (AFfilesetup setup, AFfilehandle handle)
{
	int	i;

	handle->valid = _AF_VALID_FILEHANDLE;

	if ((handle->trackCount = setup->trackCount) == 0)
		handle->tracks = NULL;
	else
	{
		handle->tracks = _af_calloc(handle->trackCount, sizeof (_Track));
		if (handle->tracks == NULL)
			return AF_FAIL;

		for (i=0; i<handle->trackCount; i++)
		{
			_Track		*track = &handle->tracks[i];
			_TrackSetup	*tracksetup = &setup->tracks[i];

			track->id = tracksetup->id;

			track->f = tracksetup->f;
			track->channelMatrix = NULL;

			/* XXXmpruett copy compression stuff too */

			if ((track->markerCount = tracksetup->markerCount) == 0)
				track->markers = NULL;
			else
			{
				int	j;

				track->markers = _af_marker_new(track->markerCount);
				if (track->markers == NULL)
					return AF_FAIL;
				for (j=0; j<track->markerCount; j++)
				{
					track->markers[j].id = tracksetup->markers[j].id;
					track->markers[j].name =
						_af_strdup(tracksetup->markers[j].name);
					if (track->markers[j].name == NULL)
						return AF_FAIL;

					track->markers[j].comment =
						_af_strdup(tracksetup->markers[j].comment);
					if (track->markers[j].comment == NULL)
						return AF_FAIL;
					track->markers[j].position = 0;
				}
			}

			track->hasAESData = tracksetup->aesDataSet;

			track->ms = NULL;
		}
	}

	/* Copy instrument data. */
	if ((handle->instrumentCount = setup->instrumentCount) == 0)
		handle->instruments = NULL;
	else
	{
		handle->instruments = _af_calloc(handle->instrumentCount,
			sizeof (_Instrument));
		if (handle->instruments == NULL)
			return AF_FAIL;

		for (i=0; i<handle->instrumentCount; i++)
		{
			int	instParamCount;

			handle->instruments[i].id = setup->instruments[i].id;

			/* Copy loops. */
			if ((handle->instruments[i].loopCount =
				setup->instruments[i].loopCount) == 0)
				handle->instruments[i].loops = NULL;
			else
			{
				int	j;

				handle->instruments[i].loops = _af_calloc(handle->instruments[i].loopCount, sizeof (_Loop));
				if (handle->instruments[i].loops == NULL)
					return AF_FAIL;
				for (j=0; j<handle->instruments[i].loopCount; j++)
				{
					_Loop	*loop;
					loop = &handle->instruments[i].loops[j];
					loop->id = setup->instruments[i].loops[j].id;
					loop->mode = AF_LOOP_MODE_NOLOOP;
					loop->count = 0;
					loop->trackid = AF_DEFAULT_TRACK;
					loop->beginMarker = 1 + (2*j);
					loop->endMarker = 2 + (2*j);
				}
			}

			/* Copy instrument parameters. */
			if ((instParamCount = _af_units[setup->fileFormat].instrumentParameterCount) == 0)
				handle->instruments[i].values = NULL;
			else
			{
				int	j;
				handle->instruments[i].values = _af_calloc(instParamCount, sizeof (AFPVu));
				if (handle->instruments[i].values == NULL)
					return AF_FAIL;
				for (j=0; j<instParamCount; j++)
				{
					handle->instruments[i].values[j] =
						_af_units[setup->fileFormat].instrumentParameters[j].defaultValue;
				}
			}
		}
	}

	/* Copy miscellaneous information. */

	if ((handle->miscellaneousCount = setup->miscellaneousCount) == 0)
		handle->miscellaneous = NULL;
	else
	{
		handle->miscellaneous = _af_calloc(handle->miscellaneousCount,
			sizeof (_Miscellaneous));
		if (handle->miscellaneous == NULL)
			return AF_FAIL;
		for (i=0; i<handle->miscellaneousCount; i++)
		{
			handle->miscellaneous[i].id = setup->miscellaneous[i].id;
			handle->miscellaneous[i].type = setup->miscellaneous[i].type;
			handle->miscellaneous[i].size = setup->miscellaneous[i].size;
			handle->miscellaneous[i].position = 0;
			handle->miscellaneous[i].buffer = NULL;
		}
	}

	return AF_SUCCEED;
}