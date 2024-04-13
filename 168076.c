nm_utils_get_testing_initialized()
{
    NMUtilsTestFlags flags;

    flags = (NMUtilsTestFlags) _nm_utils_testing;
    if (flags == NM_UTILS_TEST_NONE)
        flags = (NMUtilsTestFlags) g_atomic_int_get(&_nm_utils_testing);
    return flags != NM_UTILS_TEST_NONE;
}