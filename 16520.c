static struct dentry *proc_tgid_base_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd){
	return proc_pident_lookup(dir, dentry,
				  tgid_base_stuff, ARRAY_SIZE(tgid_base_stuff));
}