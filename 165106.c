void _af_print_tracks (AFfilehandle filehandle)
{
	int	i;
	for (i=0; i<filehandle->trackCount; i++)
	{
		_Track	*track = &filehandle->tracks[i];
		printf("track %d\n", i);
		printf(" id %d\n", track->id);
		printf(" sample format\n");
		_af_print_audioformat(&track->f);
		printf(" virtual format\n");
		_af_print_audioformat(&track->v);
		printf(" total file frames: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->totalfframes);
		printf(" total virtual frames: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->totalvframes);
		printf(" next file frame: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->nextfframe);
		printf(" next virtual frame: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->nextvframe);
		printf(" frames to ignore: %" AF_FRAMECOUNT_PRINT_FMT "\n",
			track->frames2ignore);

		printf(" data_size: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->data_size);
		printf(" fpos_first_frame: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_first_frame);
		printf(" fpos_next_frame: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_next_frame);
		printf(" fpos_after_data: %" AF_FILEOFFSET_PRINT_FMT "\n",
			track->fpos_after_data);

		printf(" channel matrix:");
		_af_print_channel_matrix(track->channelMatrix,
			track->f.channelCount, track->v.channelCount);
		printf("\n");

		printf(" marker count: %d\n", track->markerCount);
	}
}