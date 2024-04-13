set_directory_record(unsigned char *p, size_t n, struct isoent *isoent,
    struct iso9660 *iso9660, enum dir_rec_type t,
    enum vdd_type vdd_type)
{
	unsigned char *bp;
	size_t dr_len;
	size_t fi_len;

	if (p != NULL) {
		/*
		 * Check whether a write buffer size is less than the
		 * saved size which is needed to write this Directory
		 * Record.
		 */
		switch (t) {
		case DIR_REC_VD:
			dr_len = isoent->dr_len.vd; break;
		case DIR_REC_SELF:
			dr_len = isoent->dr_len.self; break;
		case DIR_REC_PARENT:
			dr_len = isoent->dr_len.parent; break;
		case DIR_REC_NORMAL:
		default:
			dr_len = isoent->dr_len.normal; break;
		}
		if (dr_len > n)
			return (0);/* Needs more buffer size. */
	}

	if (t == DIR_REC_NORMAL && isoent->identifier != NULL)
		fi_len = isoent->id_len;
	else
		fi_len = 1;

	if (p != NULL) {
		struct isoent *xisoent;
		struct isofile *file;
		unsigned char flag;

		if (t == DIR_REC_PARENT)
			xisoent = isoent->parent;
		else
			xisoent = isoent;
		file = isoent->file;
		if (file->hardlink_target != NULL)
			file = file->hardlink_target;
		/* Make a file flag. */
		if (xisoent->dir)
			flag = FILE_FLAG_DIRECTORY;
		else {
			if (file->cur_content->next != NULL)
				flag = FILE_FLAG_MULTI_EXTENT;
			else
				flag = 0;
		}

		bp = p -1;
		/* Extended Attribute Record Length */
		set_num_711(bp+2, 0);
		/* Location of Extent */
		if (xisoent->dir)
			set_num_733(bp+3, xisoent->dir_location);
		else
			set_num_733(bp+3, file->cur_content->location);
		/* Data Length */
		if (xisoent->dir)
			set_num_733(bp+11,
			    xisoent->dir_block * LOGICAL_BLOCK_SIZE);
		else
			set_num_733(bp+11, (uint32_t)file->cur_content->size);
		/* Recording Date and Time */
		/* NOTE:
		 *  If a file type is symbolic link, you are seeing this
		 *  field value is different from a value mkisofs makes.
		 *  libarchive uses lstat to get this one, but it
		 *  seems mkisofs uses stat to get.
		 */
		set_time_915(bp+19,
		    archive_entry_mtime(xisoent->file->entry));
		/* File Flags */
		bp[26] = flag;
		/* File Unit Size */
		set_num_711(bp+27, 0);
		/* Interleave Gap Size */
		set_num_711(bp+28, 0);
		/* Volume Sequence Number */
		set_num_723(bp+29, iso9660->volume_sequence_number);
		/* Length of File Identifier */
		set_num_711(bp+33, (unsigned char)fi_len);
		/* File Identifier */
		switch (t) {
		case DIR_REC_VD:
		case DIR_REC_SELF:
			set_num_711(bp+34, 0);
			break;
		case DIR_REC_PARENT:
			set_num_711(bp+34, 1);
			break;
		case DIR_REC_NORMAL:
			if (isoent->identifier != NULL)
				memcpy(bp+34, isoent->identifier, fi_len);
			else
				set_num_711(bp+34, 0);
			break;
		}
	} else
		bp = NULL;
	dr_len = 33 + fi_len;
	/* Padding Field */
	if (dr_len & 0x01) {
		dr_len ++;
		if (p != NULL)
			bp[dr_len] = 0;
	}

	/* Volume Descriptor does not record extension. */
	if (t == DIR_REC_VD) {
		if (p != NULL)
			/* Length of Directory Record */
			set_num_711(p, (unsigned char)dr_len);
		else
			isoent->dr_len.vd = (int)dr_len;
		return ((int)dr_len);
	}

	/* Rockridge */
	if (iso9660->opt.rr && vdd_type != VDD_JOLIET)
		dr_len = set_directory_record_rr(bp, (int)dr_len,
		    isoent, iso9660, t);

	if (p != NULL)
		/* Length of Directory Record */
		set_num_711(p, (unsigned char)dr_len);
	else {
		/*
		 * Save the size which is needed to write this
		 * Directory Record.
		 */
		switch (t) {
		case DIR_REC_VD:
			/* This case does not come, but compiler
			 * complains that DIR_REC_VD not handled
			 *  in switch ....  */
			break;
		case DIR_REC_SELF:
			isoent->dr_len.self = (int)dr_len; break;
		case DIR_REC_PARENT:
			isoent->dr_len.parent = (int)dr_len; break;
		case DIR_REC_NORMAL:
			isoent->dr_len.normal = (int)dr_len; break;
		}
	}

	return ((int)dr_len);
}