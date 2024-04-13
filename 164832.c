int afReadFrames (AFfilehandle file, int trackid, void *samples,
	int nvframeswanted)
{
	_Track	*track;
	SharedPtr<Module> firstmod;
	SharedPtr<Chunk> userc;
	AFframecount	nvframesleft, nvframes2read;
	int		bytes_per_vframe;
	AFframecount	vframe;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_read(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms->isDirty() && track->ms->setup(file, track) == AF_FAIL)
		return -1;

	/*if (file->seekok) {*/

	if (af_fseek(file->fh, track->fpos_next_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position read pointer at next frame");
		return -1;
	}

	/* } */

	if (track->totalvframes == -1)
		nvframes2read = nvframeswanted;
	else
	{
		nvframesleft = track->totalvframes - track->nextvframe;
		nvframes2read = (nvframeswanted > nvframesleft) ?
			nvframesleft : nvframeswanted;
	}
	bytes_per_vframe = _af_format_frame_size(&track->v, true);

	firstmod = track->ms->modules().back();
	userc = track->ms->chunks().back();

	track->filemodhappy = true;

	vframe = 0;

	if (!track->ms->mustUseAtomicNVFrames())
	{
		assert(track->frames2ignore == 0);
		userc->buffer = samples;
		userc->frameCount = nvframes2read;

		firstmod->runPull();
		if (track->filemodhappy)
			vframe += userc->frameCount;
	}
	else
	{
		bool	eof = false;

		if (track->frames2ignore != 0)
		{
			userc->frameCount = track->frames2ignore;
			userc->allocate(track->frames2ignore * bytes_per_vframe);
			if (userc->buffer == NULL)
				return 0;

			firstmod->runPull();

			/* Have we hit EOF? */
			if (userc->frameCount < track->frames2ignore)
				eof = true;

			track->frames2ignore = 0;

			free(userc->buffer);
			userc->buffer = NULL;
		}

		/*
			Now start reading useful frames, until EOF or
			premature EOF.
		*/

		while (track->filemodhappy && !eof && vframe < nvframes2read)
		{
			AFframecount	nvframes2pull;
			userc->buffer = (char *) samples + bytes_per_vframe * vframe;

			if (vframe <= nvframes2read - _AF_ATOMIC_NVFRAMES)
				nvframes2pull = _AF_ATOMIC_NVFRAMES;
			else
				nvframes2pull = nvframes2read - vframe;

			userc->frameCount = nvframes2pull;

			firstmod->runPull();

			if (track->filemodhappy)
			{
				vframe += userc->frameCount;
				if (userc->frameCount < nvframes2pull)
					eof = true;
			}
		}
	}

	track->nextvframe += vframe;

	return vframe;
}