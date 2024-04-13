_Track *_af_track_new (void)
{
	_Track *t = (_Track *) _af_malloc(sizeof (_Track));

	t->id = AF_DEFAULT_TRACK;

	t->f.compressionParams = NULL;
	t->v.compressionParams = NULL;

	t->channelMatrix = NULL;

	t->markerCount = 0;
	t->markers = NULL;

	t->hasAESData = false;
	memset(t->aesData, 0, 24);

	t->totalfframes = 0;
	t->nextfframe = 0;
	t->frames2ignore = 0;
	t->fpos_first_frame = 0;
	t->fpos_next_frame = 0;
	t->fpos_after_data = 0;
	t->totalvframes = 0;
	t->nextvframe = 0;
	t->data_size = 0;

	t->ms = NULL;

	return t;
}