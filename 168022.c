_nm_utils_set_testing(NMUtilsTestFlags flags)
{
    g_assert(!NM_FLAGS_ANY(flags, ~NM_UTILS_TEST_ALL));

    /* mask out everything except ALL, and always set GENERAL. */
    flags = (flags & NM_UTILS_TEST_ALL) | (_NM_UTILS_TEST_GENERAL | _NM_UTILS_TEST_INITIALIZED);

    if (!g_atomic_int_compare_and_exchange(&_nm_utils_testing, 0, (int) flags)) {
        /* We only allow setting _nm_utils_set_testing() once, before fetching the
         * value with nm_utils_get_testing(). */
        g_return_if_reached();
    }
}