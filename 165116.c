AFfileoffset afTellFrame (AFfilehandle file, int trackid)
{
	return afSeekFrame(file, trackid, -1);
}