nm_utils_get_testing()
{
    NMUtilsTestFlags flags;

again:
    flags = (NMUtilsTestFlags) _nm_utils_testing;
    if (flags != NM_UTILS_TEST_NONE) {
        /* Flags already initialized. Return them. */
        return flags & NM_UTILS_TEST_ALL;
    }

    /* Accessing nm_utils_get_testing() causes us to set the flags to initialized.
     * Detecting running tests also based on g_test_initialized(). */
    flags = _NM_UTILS_TEST_INITIALIZED;
    if (g_test_initialized())
        flags |= _NM_UTILS_TEST_GENERAL;

    g_atomic_int_compare_and_exchange(&_nm_utils_testing, 0, (int) flags);

    /* regardless of whether we won the race of initializing _nm_utils_testing,
     * go back and read the value again. It must be non-zero by now. */
    goto again;
}