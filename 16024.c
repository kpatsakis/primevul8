static bool mount_too_revealing(const struct super_block *sb, int *new_mnt_flags)
{
	const unsigned long required_iflags = SB_I_NOEXEC | SB_I_NODEV;
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	unsigned long s_iflags;

	if (ns->user_ns == &init_user_ns)
		return false;

	/* Can this filesystem be too revealing? */
	s_iflags = sb->s_iflags;
	if (!(s_iflags & SB_I_USERNS_VISIBLE))
		return false;

	if ((s_iflags & required_iflags) != required_iflags) {
		WARN_ONCE(1, "Expected s_iflags to contain 0x%lx\n",
			  required_iflags);
		return true;
	}

	return !mnt_already_visible(ns, sb, new_mnt_flags);
}