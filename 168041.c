_kc_exit_to_string(char *buf, int exit)
#define _kc_exit_to_string(buf, exit)                                                          \
    (G_STATIC_ASSERT_EXPR(sizeof(buf) == KC_EXIT_TO_STRING_BUF_SIZE && sizeof((buf)[0]) == 1), \
     _kc_exit_to_string(buf, exit))
{
    if (WIFEXITED(exit))
        g_snprintf(buf, KC_EXIT_TO_STRING_BUF_SIZE, "normally with status %d", WEXITSTATUS(exit));
    else if (WIFSIGNALED(exit))
        g_snprintf(buf, KC_EXIT_TO_STRING_BUF_SIZE, "by signal %d", WTERMSIG(exit));
    else
        g_snprintf(buf, KC_EXIT_TO_STRING_BUF_SIZE, "with unexpected status %d", exit);
    return buf;
}