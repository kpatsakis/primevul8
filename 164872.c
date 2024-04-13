void _af_setup_free_tracks (AFfilesetup setup)
{
	int	i;

	if (setup->tracks)
	{
		for (i=0; i<setup->trackCount; i++)
		{
			_af_setup_free_markers(setup, i);
		}

		free(setup->tracks);
	}

	setup->tracks = NULL;
	setup->trackCount = 0;
}