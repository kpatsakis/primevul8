static void print_cifs_mount_version(void)
{
	printf("mount.cifs version: %s.%s%s\n",
		MOUNT_CIFS_VERSION_MAJOR,
		MOUNT_CIFS_VERSION_MINOR,
		MOUNT_CIFS_VENDOR_SUFFIX);
}