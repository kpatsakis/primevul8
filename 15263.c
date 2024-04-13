bpf_ringbuf_restore_from_rec(struct bpf_ringbuf_hdr *hdr)
{
	unsigned long addr = (unsigned long)(void *)hdr;
	unsigned long off = (unsigned long)hdr->pg_off << PAGE_SHIFT;

	return (void*)((addr & PAGE_MASK) - off);
}