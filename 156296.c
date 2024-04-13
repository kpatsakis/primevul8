mm_init_compression(struct mm_master *mm)
{
	outgoing_stream.zalloc = (alloc_func)mm_zalloc;
	outgoing_stream.zfree = (free_func)mm_zfree;
	outgoing_stream.opaque = mm;

	incoming_stream.zalloc = (alloc_func)mm_zalloc;
	incoming_stream.zfree = (free_func)mm_zfree;
	incoming_stream.opaque = mm;
}