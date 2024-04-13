static int drop_security_capability(const struct lo_data *lo, int fd)
{
    if (!lo->xattr_security_capability) {
        /* We didn't remap the name, let the host kernel do it */
        return 0;
    }
    if (!fremovexattr(fd, lo->xattr_security_capability)) {
        /* All good */
        return 0;
    }

    switch (errno) {
    case ENODATA:
        /* Attribute didn't exist, that's fine */
        return 0;

    case ENOTSUP:
        /* FS didn't support attribute anyway, also fine */
        return 0;

    default:
        /* Hmm other error */
        return errno;
    }
}