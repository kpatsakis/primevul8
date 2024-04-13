libssh2_session_method_pref(LIBSSH2_SESSION * session, int method_type,
                            const char *prefs)
{
    char **prefvar, *s, *newprefs;
    int prefs_len = strlen(prefs);
    const LIBSSH2_COMMON_METHOD **mlist;

    switch (method_type) {
    case LIBSSH2_METHOD_KEX:
        prefvar = &session->kex_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_kex_methods;
        break;

    case LIBSSH2_METHOD_HOSTKEY:
        prefvar = &session->hostkey_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_hostkey_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_CS:
        prefvar = &session->local.crypt_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_SC:
        prefvar = &session->remote.crypt_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_MAC_CS:
        prefvar = &session->local.mac_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_MAC_SC:
        prefvar = &session->remote.mac_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_COMP_CS:
        prefvar = &session->local.comp_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **)
            _libssh2_comp_methods(session);
        break;

    case LIBSSH2_METHOD_COMP_SC:
        prefvar = &session->remote.comp_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **)
            _libssh2_comp_methods(session);
        break;

    case LIBSSH2_METHOD_LANG_CS:
        prefvar = &session->local.lang_prefs;
        mlist = NULL;
        break;

    case LIBSSH2_METHOD_LANG_SC:
        prefvar = &session->remote.lang_prefs;
        mlist = NULL;
        break;

    default:
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "Invalid parameter specified for method_type");
    }

    s = newprefs = LIBSSH2_ALLOC(session, prefs_len + 1);
    if (!newprefs) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Error allocated space for method preferences");
    }
    memcpy(s, prefs, prefs_len + 1);

    while (s && *s && mlist) {
        char *p = strchr(s, ',');
        int method_len = p ? (p - s) : (int) strlen(s);

        if (!kex_get_method_by_name(s, method_len, mlist)) {
            /* Strip out unsupported method */
            if (p) {
                memcpy(s, p + 1, strlen(s) - method_len);
            } else {
                if (s > newprefs) {
                    *(--s) = '\0';
                } else {
                    *s = '\0';
                }
            }
        }

        s = p ? (p + 1) : NULL;
    }

    if (strlen(newprefs) == 0) {
        LIBSSH2_FREE(session, newprefs);
        return _libssh2_error(session, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "The requested method(s) are not currently "
                              "supported");
    }

    if (*prefvar) {
        LIBSSH2_FREE(session, *prefvar);
    }
    *prefvar = newprefs;

    return 0;
}