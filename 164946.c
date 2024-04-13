void afSetChannelMatrix (AFfilehandle file, int trackid, double* matrix)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return;

	if (track->channelMatrix != NULL)
		free(track->channelMatrix);
	track->channelMatrix = NULL;

	if (matrix != NULL)
	{
		int	i, size;

		size = track->v.channelCount * track->f.channelCount;

		track->channelMatrix = (double *) malloc(size * sizeof (double));

		for (i = 0; i < size; i++)
			track->channelMatrix[i] = matrix[i];
	}
}