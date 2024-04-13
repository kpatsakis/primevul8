static void udf_merge_extents(struct inode *inode, struct kernel_long_ad *laarr,
			      int *endnum)
{
	int i;
	unsigned long blocksize = inode->i_sb->s_blocksize;
	unsigned char blocksize_bits = inode->i_sb->s_blocksize_bits;

	for (i = 0; i < (*endnum - 1); i++) {
		struct kernel_long_ad *li /*l[i]*/ = &laarr[i];
		struct kernel_long_ad *lip1 /*l[i plus 1]*/ = &laarr[i + 1];

		if (((li->extLength >> 30) == (lip1->extLength >> 30)) &&
			(((li->extLength >> 30) ==
				(EXT_NOT_RECORDED_NOT_ALLOCATED >> 30)) ||
			((lip1->extLocation.logicalBlockNum -
			  li->extLocation.logicalBlockNum) ==
			(((li->extLength & UDF_EXTENT_LENGTH_MASK) +
			blocksize - 1) >> blocksize_bits)))) {

			if (((li->extLength & UDF_EXTENT_LENGTH_MASK) +
				(lip1->extLength & UDF_EXTENT_LENGTH_MASK) +
				blocksize - 1) & ~UDF_EXTENT_LENGTH_MASK) {
				lip1->extLength = (lip1->extLength -
						  (li->extLength &
						   UDF_EXTENT_LENGTH_MASK) +
						   UDF_EXTENT_LENGTH_MASK) &
							~(blocksize - 1);
				li->extLength = (li->extLength &
						 UDF_EXTENT_FLAG_MASK) +
						(UDF_EXTENT_LENGTH_MASK + 1) -
						blocksize;
				lip1->extLocation.logicalBlockNum =
					li->extLocation.logicalBlockNum +
					((li->extLength &
						UDF_EXTENT_LENGTH_MASK) >>
						blocksize_bits);
			} else {
				li->extLength = lip1->extLength +
					(((li->extLength &
						UDF_EXTENT_LENGTH_MASK) +
					 blocksize - 1) & ~(blocksize - 1));
				if (*endnum > (i + 2))
					memmove(&laarr[i + 1], &laarr[i + 2],
						sizeof(struct long_ad) *
						(*endnum - (i + 2)));
				i--;
				(*endnum)--;
			}
		} else if (((li->extLength >> 30) ==
				(EXT_NOT_RECORDED_ALLOCATED >> 30)) &&
			   ((lip1->extLength >> 30) ==
				(EXT_NOT_RECORDED_NOT_ALLOCATED >> 30))) {
			udf_free_blocks(inode->i_sb, inode, &li->extLocation, 0,
					((li->extLength &
					  UDF_EXTENT_LENGTH_MASK) +
					 blocksize - 1) >> blocksize_bits);
			li->extLocation.logicalBlockNum = 0;
			li->extLocation.partitionReferenceNum = 0;

			if (((li->extLength & UDF_EXTENT_LENGTH_MASK) +
			     (lip1->extLength & UDF_EXTENT_LENGTH_MASK) +
			     blocksize - 1) & ~UDF_EXTENT_LENGTH_MASK) {
				lip1->extLength = (lip1->extLength -
						   (li->extLength &
						   UDF_EXTENT_LENGTH_MASK) +
						   UDF_EXTENT_LENGTH_MASK) &
						   ~(blocksize - 1);
				li->extLength = (li->extLength &
						 UDF_EXTENT_FLAG_MASK) +
						(UDF_EXTENT_LENGTH_MASK + 1) -
						blocksize;
			} else {
				li->extLength = lip1->extLength +
					(((li->extLength &
						UDF_EXTENT_LENGTH_MASK) +
					  blocksize - 1) & ~(blocksize - 1));
				if (*endnum > (i + 2))
					memmove(&laarr[i + 1], &laarr[i + 2],
						sizeof(struct long_ad) *
						(*endnum - (i + 2)));
				i--;
				(*endnum)--;
			}
		} else if ((li->extLength >> 30) ==
					(EXT_NOT_RECORDED_ALLOCATED >> 30)) {
			udf_free_blocks(inode->i_sb, inode,
					&li->extLocation, 0,
					((li->extLength &
						UDF_EXTENT_LENGTH_MASK) +
					 blocksize - 1) >> blocksize_bits);
			li->extLocation.logicalBlockNum = 0;
			li->extLocation.partitionReferenceNum = 0;
			li->extLength = (li->extLength &
						UDF_EXTENT_LENGTH_MASK) |
						EXT_NOT_RECORDED_NOT_ALLOCATED;
		}
	}
}