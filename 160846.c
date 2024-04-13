initialize(void)
{
    /* The data buffer is allocated somewhat larger, so that
     * we can use this extra space (which is allocated in secure memory)
     * as a temporary hash buffer */
    rndpool = secure_alloc ? xmalloc_secure_clear(POOLSIZE+BLOCKLEN)
			   : xmalloc_clear(POOLSIZE+BLOCKLEN);
    keypool = secure_alloc ? xmalloc_secure_clear(POOLSIZE+BLOCKLEN)
			   : xmalloc_clear(POOLSIZE+BLOCKLEN);
    is_initialized = 1;
}