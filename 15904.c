static inline bool may_mandlock(void)
{
	return capable(CAP_SYS_ADMIN);
}