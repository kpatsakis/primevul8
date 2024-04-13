static uint32_t generate_seed() {
    uint32_t seed;
    int done = 0;

#if !defined(_WIN32) && defined(USE_URANDOM)
    if (!done && seed_from_urandom(&seed) == 0)
        done = 1;
#endif

#if defined(_WIN32) && defined(USE_WINDOWS_CRYPTOAPI)
    if (!done && seed_from_windows_cryptoapi(&seed) == 0)
        done = 1;
#endif

    if (!done) {
        /* Fall back to timestamp and PID if no better randomness is
           available */
        seed_from_timestamp_and_pid(&seed);
    }

    /* Make sure the seed is never zero */
    if (seed == 0)
        seed = 1;

    return seed;
}