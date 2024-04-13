bool unmap_file(void *start, size_t size)
{
#ifdef HAVE_MMAP
	if (munmap( start, size ) != 0) {
		DEBUG( 1, ("map_file: Failed to unmap address %p "
			"of size %u - %s\n", 
			start, (unsigned int)size, strerror(errno) ));
		return false;
	}
	return true;
#else
	talloc_free(start);
	return true;
#endif
}