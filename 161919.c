virt_to_phys_or_null_size(void *va, unsigned long size)
{
	bool bad_size;

	if (!va)
		return 0;

	if (virt_addr_valid(va))
		return virt_to_phys(va);

	/*
	 * A fully aligned variable on the stack is guaranteed not to
	 * cross a page bounary. Try to catch strings on the stack by
	 * checking that 'size' is a power of two.
	 */
	bad_size = size > PAGE_SIZE || !is_power_of_2(size);

	WARN_ON(!IS_ALIGNED((unsigned long)va, size) || bad_size);

	return slow_virt_to_phys(va);
}