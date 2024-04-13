static void freeFileHandle (AFfilehandle filehandle)
{
	int	fileFormat;
	if (filehandle == NULL || filehandle->valid != _AF_VALID_FILEHANDLE)
	{
		_af_error(AF_BAD_FILEHANDLE, "bad filehandle");
		return;
	}

	filehandle->valid = 0;

	if (filehandle->fileName != NULL)
		free(filehandle->fileName);

	fileFormat = filehandle->fileFormat;

	if (filehandle->formatSpecific != NULL)
	{
		free(filehandle->formatSpecific);
		filehandle->formatSpecific = NULL;
	}

	if (filehandle->tracks)
	{
		int	i;
		for (i=0; i<filehandle->trackCount; i++)
		{
			/* Free the compression parameters. */
			if (filehandle->tracks[i].f.compressionParams)
			{
				AUpvfree((AUpvlist) filehandle->tracks[i].f.compressionParams);
				filehandle->tracks[i].f.compressionParams = AU_NULL_PVLIST;
			}

			if (filehandle->tracks[i].v.compressionParams)
			{
				AUpvfree((AUpvlist) filehandle->tracks[i].v.compressionParams);
				filehandle->tracks[i].v.compressionParams = AU_NULL_PVLIST;
			}

			/* Free the track's modules. */

			delete filehandle->tracks[i].ms;
			filehandle->tracks[i].ms = NULL;

			if (filehandle->tracks[i].channelMatrix)
			{
				free(filehandle->tracks[i].channelMatrix);
				filehandle->tracks[i].channelMatrix = NULL;
			}

			if (filehandle->tracks[i].markers)
			{
				int	j;
				for (j=0; j<filehandle->tracks[i].markerCount; j++)
				{
					if (filehandle->tracks[i].markers[j].name)
					{
						free(filehandle->tracks[i].markers[j].name);
						filehandle->tracks[i].markers[j].name = NULL;
					}
					if (filehandle->tracks[i].markers[j].comment)
					{
						free(filehandle->tracks[i].markers[j].comment);
						filehandle->tracks[i].markers[j].comment = NULL;
					}

				}

				free(filehandle->tracks[i].markers);
				filehandle->tracks[i].markers = NULL;
			}
		}

		free(filehandle->tracks);
		filehandle->tracks = NULL;
	}
	filehandle->trackCount = 0;

	if (filehandle->instruments)
	{
		int	i;
		for (i=0; i<filehandle->instrumentCount; i++)
		{
			if (filehandle->instruments[i].loops)
			{
				free(filehandle->instruments[i].loops);
				filehandle->instruments[i].loops = NULL;
			}
			filehandle->instruments[i].loopCount = 0;

			if (filehandle->instruments[i].values)
			{
				freeInstParams(filehandle->instruments[i].values, fileFormat);
				filehandle->instruments[i].values = NULL;
			}
		}
		free(filehandle->instruments);
		filehandle->instruments = NULL;
	}
	filehandle->instrumentCount = 0;

	if (filehandle->miscellaneous)
	{
		free(filehandle->miscellaneous);
		filehandle->miscellaneous = NULL;
	}
	filehandle->miscellaneousCount = 0;

	memset(filehandle, 0, sizeof (_AFfilehandle));
	free(filehandle);
}