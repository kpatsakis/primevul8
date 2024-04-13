static inline int is_extended(struct dos_partition *p)
{
	return (p->sys_ind == MBR_DOS_EXTENDED_PARTITION ||
		p->sys_ind == MBR_W95_EXTENDED_PARTITION ||
		p->sys_ind == MBR_LINUX_EXTENDED_PARTITION);
}