fd_boot_image_size(int media_type)
{
	switch (media_type) {
	case BOOT_MEDIA_1_2M_DISKETTE:
		return (FD_1_2M_SIZE);
	case BOOT_MEDIA_1_44M_DISKETTE:
		return (FD_1_44M_SIZE);
	case BOOT_MEDIA_2_88M_DISKETTE:
		return (FD_2_88M_SIZE);
	default:
		return (0);
	}
}