static int selinux_set_mnt_opts(struct super_block *sb,
				struct security_mnt_opts *opts,
				unsigned long kern_flags,
				unsigned long *set_kern_flags)
{
	const struct cred *cred = current_cred();
	int rc = 0, i;
	struct superblock_security_struct *sbsec = sb->s_security;
	const char *name = sb->s_type->name;
	struct inode *inode = sbsec->sb->s_root->d_inode;
	struct inode_security_struct *root_isec = inode->i_security;
	u32 fscontext_sid = 0, context_sid = 0, rootcontext_sid = 0;
	u32 defcontext_sid = 0;
	char **mount_options = opts->mnt_opts;
	int *flags = opts->mnt_opts_flags;
	int num_opts = opts->num_mnt_opts;

	mutex_lock(&sbsec->lock);

	if (!ss_initialized) {
		if (!num_opts) {
			/* Defer initialization until selinux_complete_init,
			   after the initial policy is loaded and the security
			   server is ready to handle calls. */
			goto out;
		}
		rc = -EINVAL;
		printk(KERN_WARNING "SELinux: Unable to set superblock options "
			"before the security server is initialized\n");
		goto out;
	}
	if (kern_flags && !set_kern_flags) {
		/* Specifying internal flags without providing a place to
		 * place the results is not allowed */
		rc = -EINVAL;
		goto out;
	}

	/*
	 * Binary mount data FS will come through this function twice.  Once
	 * from an explicit call and once from the generic calls from the vfs.
	 * Since the generic VFS calls will not contain any security mount data
	 * we need to skip the double mount verification.
	 *
	 * This does open a hole in which we will not notice if the first
	 * mount using this sb set explict options and a second mount using
	 * this sb does not set any security options.  (The first options
	 * will be used for both mounts)
	 */
	if ((sbsec->flags & SE_SBINITIALIZED) && (sb->s_type->fs_flags & FS_BINARY_MOUNTDATA)
	    && (num_opts == 0))
		goto out;

	/*
	 * parse the mount options, check if they are valid sids.
	 * also check if someone is trying to mount the same sb more
	 * than once with different security options.
	 */
	for (i = 0; i < num_opts; i++) {
		u32 sid;

		if (flags[i] == SBLABEL_MNT)
			continue;
		rc = security_context_to_sid(mount_options[i],
					     strlen(mount_options[i]), &sid, GFP_KERNEL);
		if (rc) {
			printk(KERN_WARNING "SELinux: security_context_to_sid"
			       "(%s) failed for (dev %s, type %s) errno=%d\n",
			       mount_options[i], sb->s_id, name, rc);
			goto out;
		}
		switch (flags[i]) {
		case FSCONTEXT_MNT:
			fscontext_sid = sid;

			if (bad_option(sbsec, FSCONTEXT_MNT, sbsec->sid,
					fscontext_sid))
				goto out_double_mount;

			sbsec->flags |= FSCONTEXT_MNT;
			break;
		case CONTEXT_MNT:
			context_sid = sid;

			if (bad_option(sbsec, CONTEXT_MNT, sbsec->mntpoint_sid,
					context_sid))
				goto out_double_mount;

			sbsec->flags |= CONTEXT_MNT;
			break;
		case ROOTCONTEXT_MNT:
			rootcontext_sid = sid;

			if (bad_option(sbsec, ROOTCONTEXT_MNT, root_isec->sid,
					rootcontext_sid))
				goto out_double_mount;

			sbsec->flags |= ROOTCONTEXT_MNT;

			break;
		case DEFCONTEXT_MNT:
			defcontext_sid = sid;

			if (bad_option(sbsec, DEFCONTEXT_MNT, sbsec->def_sid,
					defcontext_sid))
				goto out_double_mount;

			sbsec->flags |= DEFCONTEXT_MNT;

			break;
		default:
			rc = -EINVAL;
			goto out;
		}
	}

	if (sbsec->flags & SE_SBINITIALIZED) {
		/* previously mounted with options, but not on this attempt? */
		if ((sbsec->flags & SE_MNTMASK) && !num_opts)
			goto out_double_mount;
		rc = 0;
		goto out;
	}

	if (strcmp(sb->s_type->name, "proc") == 0)
		sbsec->flags |= SE_SBPROC;

	if (!sbsec->behavior) {
		/*
		 * Determine the labeling behavior to use for this
		 * filesystem type.
		 */
		rc = security_fs_use(sb);
		if (rc) {
			printk(KERN_WARNING
				"%s: security_fs_use(%s) returned %d\n",
					__func__, sb->s_type->name, rc);
			goto out;
		}
	}
	/* sets the context of the superblock for the fs being mounted. */
	if (fscontext_sid) {
		rc = may_context_mount_sb_relabel(fscontext_sid, sbsec, cred);
		if (rc)
			goto out;

		sbsec->sid = fscontext_sid;
	}

	/*
	 * Switch to using mount point labeling behavior.
	 * sets the label used on all file below the mountpoint, and will set
	 * the superblock context if not already set.
	 */
	if (kern_flags & SECURITY_LSM_NATIVE_LABELS && !context_sid) {
		sbsec->behavior = SECURITY_FS_USE_NATIVE;
		*set_kern_flags |= SECURITY_LSM_NATIVE_LABELS;
	}

	if (context_sid) {
		if (!fscontext_sid) {
			rc = may_context_mount_sb_relabel(context_sid, sbsec,
							  cred);
			if (rc)
				goto out;
			sbsec->sid = context_sid;
		} else {
			rc = may_context_mount_inode_relabel(context_sid, sbsec,
							     cred);
			if (rc)
				goto out;
		}
		if (!rootcontext_sid)
			rootcontext_sid = context_sid;

		sbsec->mntpoint_sid = context_sid;
		sbsec->behavior = SECURITY_FS_USE_MNTPOINT;
	}

	if (rootcontext_sid) {
		rc = may_context_mount_inode_relabel(rootcontext_sid, sbsec,
						     cred);
		if (rc)
			goto out;

		root_isec->sid = rootcontext_sid;
		root_isec->initialized = 1;
	}

	if (defcontext_sid) {
		if (sbsec->behavior != SECURITY_FS_USE_XATTR &&
			sbsec->behavior != SECURITY_FS_USE_NATIVE) {
			rc = -EINVAL;
			printk(KERN_WARNING "SELinux: defcontext option is "
			       "invalid for this filesystem type\n");
			goto out;
		}

		if (defcontext_sid != sbsec->def_sid) {
			rc = may_context_mount_inode_relabel(defcontext_sid,
							     sbsec, cred);
			if (rc)
				goto out;
		}

		sbsec->def_sid = defcontext_sid;
	}

	rc = sb_finish_set_opts(sb);
out:
	mutex_unlock(&sbsec->lock);
	return rc;
out_double_mount:
	rc = -EINVAL;
	printk(KERN_WARNING "SELinux: mount invalid.  Same superblock, different "
	       "security settings for (dev %s, type %s)\n", sb->s_id, name);
	goto out;
}