urlAppendDomain(char *host)
{
    /* For IPv4 addresses check for a dot */
    /* For IPv6 addresses also check for a colon */
    if (Config.appendDomain && !strchr(host, '.') && !strchr(host, ':')) {
        const uint64_t dlen = strlen(host);
        const uint64_t want = dlen + Config.appendDomainLen;
        if (want > SQUIDHOSTNAMELEN - 1) {
            debugs(23, 2, "URL domain too large (" << dlen << " bytes)");
            return false;
        }
        strncat(host, Config.appendDomain, SQUIDHOSTNAMELEN - dlen - 1);
    }
    return true;
}