wait_on_child (int pid)
{
        int status;

 wait_again:
        if (waitpid (pid, &status, 0) < 0) {
                if (errno == EINTR) {
                        goto wait_again;
                } else if (errno == ECHILD) {
                        ; /* do nothing, child already reaped */
                } else {
                        gs_debug ("waitpid () should not fail in 'GSWindow'");
                }
        }

        return status;
}