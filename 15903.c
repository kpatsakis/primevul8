struct vfsmount *fc_mount(struct fs_context *fc)
{
	int err = vfs_get_tree(fc);
	if (!err) {
		up_write(&fc->root->d_sb->s_umount);
		return vfs_create_mount(fc);
	}
	return ERR_PTR(err);
}