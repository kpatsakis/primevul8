AFfilesetup _af_filesetup_copy (AFfilesetup setup, AFfilesetup defaultSetup,
	bool copyMarks)
{
	AFfilesetup	newsetup;
	int		i;
	int		instrumentCount, miscellaneousCount, trackCount;

	newsetup = _af_malloc(sizeof (_AFfilesetup));
	if (newsetup == AF_NULL_FILESETUP)
		return AF_NULL_FILESETUP;

#ifdef DEBUG
	printf("default: trackset=%d instset=%d miscset=%d\n",
		defaultSetup->trackSet, defaultSetup->instrumentSet,
		defaultSetup->miscellaneousSet);
	printf("setup: trackset=%d instset=%d miscset=%d\n",
		setup->trackSet, setup->instrumentSet, setup->miscellaneousSet);
#endif

	*newsetup = *defaultSetup;

	newsetup->tracks = NULL;
	newsetup->instruments = NULL;
	newsetup->miscellaneous = NULL;

	/* Copy tracks. */
	trackCount = setup->trackSet ? setup->trackCount :
		newsetup->trackSet ? newsetup->trackCount : 0;
	alloccopy(_TrackSetup, trackCount, newsetup->tracks, setup->tracks);
	newsetup->trackCount = trackCount;

	/* Copy instruments. */
	instrumentCount = setup->instrumentSet ? setup->instrumentCount :
		newsetup->instrumentSet ? newsetup->instrumentCount : 0;
	alloccopy(_InstrumentSetup, instrumentCount, newsetup->instruments, setup->instruments);
	newsetup->instrumentCount = instrumentCount;

	/* Copy miscellaneous information. */
	miscellaneousCount = setup->miscellaneousSet ? setup->miscellaneousCount :
		newsetup->miscellaneousSet ? newsetup->miscellaneousCount : 0;
	alloccopy(_MiscellaneousSetup, miscellaneousCount, newsetup->miscellaneous, setup->miscellaneous);
	newsetup->miscellaneousCount = miscellaneousCount;

	for (i=0; i<setup->trackCount; i++)
	{
		int	j;
		_TrackSetup	*track = &newsetup->tracks[i];

		/* XXXmpruett set compression information */

		if (!setup->tracks[i].markersSet && !copyMarks)
		{
			track->markers = NULL;
			track->markerCount = 0;
			continue;
		}

		alloccopy(_MarkerSetup, setup->tracks[i].markerCount,
			track->markers, setup->tracks[i].markers);
		track->markerCount = setup->tracks[i].markerCount;

		for (j=0; j<setup->tracks[i].markerCount; j++)
		{
			track->markers[j].name =
				_af_strdup(setup->tracks[i].markers[j].name);
			if (track->markers[j].name == NULL)
				goto fail;

			track->markers[j].comment =
				_af_strdup(setup->tracks[i].markers[j].comment);
			if (track->markers[j].comment == NULL)
				goto fail;
		}
	}

	for (i=0; i<newsetup->instrumentCount; i++)
	{
		_InstrumentSetup	*instrument = &newsetup->instruments[i];
		alloccopy(_LoopSetup, setup->instruments[i].loopCount,
			instrument->loops, setup->instruments[i].loops);
	}

	return newsetup;

	fail:
		if (newsetup->miscellaneous)
			free(newsetup->miscellaneous);
		if (newsetup->instruments)
			free(newsetup->instruments);
		if (newsetup->tracks)
			free(newsetup->tracks);
		if (newsetup)
			free(newsetup);

	return AF_NULL_FILESETUP;
}