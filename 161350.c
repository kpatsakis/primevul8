already_done_initial_setup_on_this_boot (void)
{
        if (g_file_test (ALREADY_RAN_INITIAL_SETUP_ON_THIS_BOOT, G_FILE_TEST_EXISTS))
                return TRUE;

        return FALSE;
}