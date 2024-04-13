static bool work_is_static_object(void *addr)
{
	struct work_struct *work = addr;

	return test_bit(WORK_STRUCT_STATIC_BIT, work_data_bits(work));
}