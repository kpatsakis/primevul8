static void wrstr(const int f, void * const tls_fd, const char *s)
{
    static char outbuf[CONF_TCP_SO_SNDBUF];
    static size_t outcnt;
    size_t l;

    if (s == NULL) {
        if (outcnt > (size_t) 0U) {
#ifdef WITH_TLS
            if (tls_fd != NULL) {
                if (secure_safe_write(tls_fd, outbuf, outcnt) !=
                    (ssize_t) outcnt) {
                    return;
                }
            } else
#endif
            {
                (void) tls_fd;
                if (safe_write(f, outbuf, outcnt, -1) != (ssize_t) outcnt) {
                    return;
                }
            }
        }
        outcnt = (size_t) 0U;
        return;
    }
    if ((l = strlen(s)) <= (size_t) 0U) {
        return;
    }
    if (l <= (sizeof outbuf - outcnt)) {
        memcpy(outbuf + outcnt, s, l); /* secure, see above */
        outcnt += l;
        return;
    }
    if (outcnt < sizeof outbuf) {
        const size_t rest = sizeof outbuf - outcnt;

        memcpy(outbuf + outcnt, s, rest);   /* secure, see above */
        s += rest;
        l -= rest;
    }
#ifdef WITH_TLS
    if (tls_fd != NULL) {
        if (secure_safe_write(tls_fd, outbuf, sizeof outbuf) !=
            (ssize_t) sizeof outbuf) {
            return;
        }
    } else
#endif
    {
        if (safe_write(f, outbuf, sizeof outbuf, -1) !=
            (ssize_t) sizeof outbuf) {
            return;
        }
    }
#ifdef WITH_TLS
    if (tls_fd != NULL) {
        while (l > sizeof outbuf) {
            if (secure_safe_write(tls_fd, s, sizeof outbuf) !=
                (ssize_t) sizeof outbuf) {
                return;
            }
            s += sizeof outbuf;
            l -= sizeof outbuf;
        }
    } else
#endif
    {
        while (l > sizeof outbuf) {
            if (safe_write(f, s, sizeof outbuf, -1) !=
                (ssize_t) sizeof outbuf) {
                return;
            }
            s += sizeof outbuf;
            l -= sizeof outbuf;
        }
    }
    if (l > (size_t) 0U) {
        memcpy(outbuf, s, l);          /* safe, l <= sizeof outbuf */
        outcnt = l;
    }
}