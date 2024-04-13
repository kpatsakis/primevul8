static void parse_xattrmap_map(struct lo_data *lo,
                               const char *rule, char sep)
{
    const char *tmp;
    char *key;
    char *prefix;
    XattrMapEntry tmp_entry;

    if (*rule != sep) {
        fuse_log(FUSE_LOG_ERR,
                 "%s: Expecting '%c' after 'map' keyword, found '%c'\n",
                 __func__, sep, *rule);
        exit(1);
    }

    rule++;

    /* At start of 'key' field */
    tmp = strchr(rule, sep);
    if (!tmp) {
        fuse_log(FUSE_LOG_ERR,
                 "%s: Missing '%c' at end of key field in map rule\n",
                 __func__, sep);
        exit(1);
    }

    key = g_strndup(rule, tmp - rule);
    rule = tmp + 1;

    /* At start of prefix field */
    tmp = strchr(rule, sep);
    if (!tmp) {
        fuse_log(FUSE_LOG_ERR,
                 "%s: Missing '%c' at end of prefix field in map rule\n",
                 __func__, sep);
        exit(1);
    }

    prefix = g_strndup(rule, tmp - rule);
    rule = tmp + 1;

    /*
     * This should be the end of the string, we don't allow
     * any more commands after 'map'.
     */
    if (*rule) {
        fuse_log(FUSE_LOG_ERR,
                 "%s: Expecting end of command after map, found '%c'\n",
                 __func__, *rule);
        exit(1);
    }

    /* 1st: Prefix matches/everything */
    tmp_entry.flags = XATTR_MAP_FLAG_PREFIX | XATTR_MAP_FLAG_ALL;
    tmp_entry.key = g_strdup(key);
    tmp_entry.prepend = g_strdup(prefix);
    add_xattrmap_entry(lo, &tmp_entry);

    if (!*key) {
        /* Prefix all case */

        /* 2nd: Hide any non-prefixed entries on the host */
        tmp_entry.flags = XATTR_MAP_FLAG_BAD | XATTR_MAP_FLAG_ALL;
        tmp_entry.key = g_strdup("");
        tmp_entry.prepend = g_strdup("");
        add_xattrmap_entry(lo, &tmp_entry);
    } else {
        /* Prefix matching case */

        /* 2nd: Hide non-prefixed but matching entries on the host */
        tmp_entry.flags = XATTR_MAP_FLAG_BAD | XATTR_MAP_FLAG_SERVER;
        tmp_entry.key = g_strdup(""); /* Not used */
        tmp_entry.prepend = g_strdup(key);
        add_xattrmap_entry(lo, &tmp_entry);

        /* 3rd: Stop the client accessing prefixed attributes directly */
        tmp_entry.flags = XATTR_MAP_FLAG_BAD | XATTR_MAP_FLAG_CLIENT;
        tmp_entry.key = g_strdup(prefix);
        tmp_entry.prepend = g_strdup(""); /* Not used */
        add_xattrmap_entry(lo, &tmp_entry);

        /* 4th: Everything else is OK */
        tmp_entry.flags = XATTR_MAP_FLAG_OK | XATTR_MAP_FLAG_ALL;
        tmp_entry.key = g_strdup("");
        tmp_entry.prepend = g_strdup("");
        add_xattrmap_entry(lo, &tmp_entry);
    }

    g_free(key);
    g_free(prefix);
}