int afWriteFrames (AFfilehandle file, int trackid, const void *samples,
	int nvframes2write)
{
	SharedPtr<Module> firstmod;
	SharedPtr<Chunk> userc;
	_Track *track;
	int bytes_per_vframe;
	AFframecount vframe;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_write(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms->isDirty() && track->ms->setup(file, track) == AF_FAIL)
		return -1;

	/*if (file->seekok) {*/

	if (af_fseek(file->fh, track->fpos_next_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position write pointer at next frame");
		return -1;
	}

	/* } */

	bytes_per_vframe = _af_format_frame_size(&track->v, true);

	firstmod = track->ms->modules().front();
	userc = track->ms->chunks().front();

	track->filemodhappy = true;

	vframe = 0;
#ifdef UNLIMITED_CHUNK_NVFRAMES
	/*
		OPTIMIZATION: see the comment at the very end of
		arrangemodules() in modules.c for an explanation of this:
	*/
	if (!trk->ms->mustUseAtomicNVFrames())
	{
		userc->buffer = (char *) samples;
		userc->frameCount = nvframes2write;

		firstmod->runPush();

		/* Count this chunk if there was no i/o error. */
		if (trk->filemodhappy)
			vframe += userc->frameCount;
	}
	else
#else
	/* Optimization must be off. */
	assert(track->ms->mustUseAtomicNVFrames());
#endif
	{
		while (vframe < nvframes2write)
		{
			userc->buffer = (char *) samples + bytes_per_vframe * vframe;
			if (vframe <= nvframes2write - _AF_ATOMIC_NVFRAMES)
				userc->frameCount = _AF_ATOMIC_NVFRAMES;
			else
				userc->frameCount = nvframes2write - vframe;

			firstmod->runPush();

			if (!track->filemodhappy)
				break;

			vframe += userc->frameCount;
		}
	}

	track->nextvframe += vframe;
	track->totalvframes += vframe;

	return vframe;
}