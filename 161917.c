static void *realloc_pages(void *old_memmap, int old_shift)
{
	void *ret;

	ret = (void *)__get_free_pages(GFP_KERNEL, old_shift + 1);
	if (!ret)
		goto out;

	/*
	 * A first-time allocation doesn't have anything to copy.
	 */
	if (!old_memmap)
		return ret;

	memcpy(ret, old_memmap, PAGE_SIZE << old_shift);

out:
	free_pages((unsigned long)old_memmap, old_shift);
	return ret;
}