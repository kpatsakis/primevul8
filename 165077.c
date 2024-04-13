void _af_setup_free_markers (AFfilesetup setup, int trackno)
{
	if (setup->tracks[trackno].markerCount != 0)
	{
		int	i;
		for (i=0; i<setup->tracks[trackno].markerCount; i++)
		{
			free(setup->tracks[trackno].markers[i].name);
			free(setup->tracks[trackno].markers[i].comment);
		}

		free(setup->tracks[trackno].markers);
	}

	setup->tracks[trackno].markers = NULL;
	setup->tracks[trackno].markerCount = 0;
}