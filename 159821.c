static inline bool drive_no_geom(int drive)
{
	return !current_type[drive] && !ITYPE(UDRS->fd_device);
}