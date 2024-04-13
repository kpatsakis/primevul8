static int seed_from_timestamp_and_pid(uint32_t *seed) {
#ifdef HAVE_GETTIMEOFDAY
    /* XOR of seconds and microseconds */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *seed = (uint32_t)tv.tv_sec ^ (uint32_t)tv.tv_usec;
#else
    /* Seconds only */
    *seed = (uint32_t)time(NULL);
#endif

    /* XOR with PID for more randomness */
#if defined(_WIN32)
    *seed ^= (uint32_t)_getpid();
#elif defined(HAVE_GETPID)
    *seed ^= (uint32_t)getpid();
#endif

    return 0;
}