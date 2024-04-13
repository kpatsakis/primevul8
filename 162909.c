suspend(int signo, struct sudo_conv_callback *callback)
{
    int ret = 0;
    debug_decl(suspend, SUDO_DEBUG_CONV);

    if (callback != NULL && SUDO_API_VERSION_GET_MAJOR(callback->version) != SUDO_CONV_CALLBACK_VERSION_MAJOR) {
	sudo_debug_printf(SUDO_DEBUG_WARN|SUDO_DEBUG_LINENO,
	    "callback major version mismatch, expected %u, got %u",
	    SUDO_CONV_CALLBACK_VERSION_MAJOR,
	    SUDO_API_VERSION_GET_MAJOR(callback->version));
	callback = NULL;
    }

    if (callback != NULL && callback->on_suspend != NULL) {
	if (callback->on_suspend(signo, callback->closure) == -1)
	    ret = -1;
    }
    kill(getpid(), signo);
    if (callback != NULL && callback->on_resume != NULL) {
	if (callback->on_resume(signo, callback->closure) == -1)
	    ret = -1;
    }
    debug_return_int(ret);
}