static void mnt_warn_timestamp_expiry(struct path *mountpoint, struct vfsmount *mnt)
{
	struct super_block *sb = mnt->mnt_sb;

	if (!__mnt_is_readonly(mnt) &&
	   (ktime_get_real_seconds() + TIME_UPTIME_SEC_MAX > sb->s_time_max)) {
		char *buf = (char *)__get_free_page(GFP_KERNEL);
		char *mntpath = buf ? d_path(mountpoint, buf, PAGE_SIZE) : ERR_PTR(-ENOMEM);
		struct tm tm;

		time64_to_tm(sb->s_time_max, 0, &tm);

		pr_warn("%s filesystem being %s at %s supports timestamps until %04ld (0x%llx)\n",
			sb->s_type->name,
			is_mounted(mnt) ? "remounted" : "mounted",
			mntpath,
			tm.tm_year+1900, (unsigned long long)sb->s_time_max);

		free_page((unsigned long)buf);
	}
}