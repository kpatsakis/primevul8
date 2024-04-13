GIT_INLINE(bool) is_filemode_changed(git_filemode_t a, git_filemode_t b, int respect_filemode)
{
	/* If core.filemode = false, ignore links in the repository and executable bit changes */
	if (!respect_filemode) {
		if (a == S_IFLNK)
			a = GIT_FILEMODE_BLOB;
		if (b == S_IFLNK)
			b = GIT_FILEMODE_BLOB;

		a &= ~0111;
		b &= ~0111;
	}

	return (a != b);
}