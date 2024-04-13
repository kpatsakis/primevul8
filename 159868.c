static int compat_setdrvprm(int drive,
			    struct compat_floppy_drive_params __user *arg)
{
	struct compat_floppy_drive_params v;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (copy_from_user(&v, arg, sizeof(struct compat_floppy_drive_params)))
		return -EFAULT;
	if (!valid_floppy_drive_params(v.autodetect, v.native_format))
		return -EINVAL;
	mutex_lock(&floppy_mutex);
	UDP->cmos = v.cmos;
	UDP->max_dtr = v.max_dtr;
	UDP->hlt = v.hlt;
	UDP->hut = v.hut;
	UDP->srt = v.srt;
	UDP->spinup = v.spinup;
	UDP->spindown = v.spindown;
	UDP->spindown_offset = v.spindown_offset;
	UDP->select_delay = v.select_delay;
	UDP->rps = v.rps;
	UDP->tracks = v.tracks;
	UDP->timeout = v.timeout;
	UDP->interleave_sect = v.interleave_sect;
	UDP->max_errors = v.max_errors;
	UDP->flags = v.flags;
	UDP->read_track = v.read_track;
	memcpy(UDP->autodetect, v.autodetect, sizeof(v.autodetect));
	UDP->checkfreq = v.checkfreq;
	UDP->native_format = v.native_format;
	mutex_unlock(&floppy_mutex);
	return 0;
}