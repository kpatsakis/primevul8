match_config_eval(const char *str, const char *tag, guint cur_nm_version)
{
    gs_free char *     s_ver        = NULL;
    gs_strfreev char **s_ver_tokens = NULL;
    int                v_maj = -1, v_min = -1, v_mic = -1;
    guint              c_maj = -1, c_min = -1, c_mic = -1;
    guint              n_tokens;

    s_ver = g_strdup(str);
    g_strstrip(s_ver);

    /* Let's be strict with the accepted format here. No funny stuff!! */

    if (s_ver[strspn(s_ver, ".0123456789")] != '\0')
        return FALSE;

    s_ver_tokens = g_strsplit(s_ver, ".", -1);
    n_tokens     = g_strv_length(s_ver_tokens);
    if (n_tokens == 0 || n_tokens > 3)
        return FALSE;

    v_maj = _nm_utils_ascii_str_to_int64(s_ver_tokens[0], 10, 0, 0xFFFF, -1);
    if (v_maj < 0)
        return FALSE;
    if (n_tokens >= 2) {
        v_min = _nm_utils_ascii_str_to_int64(s_ver_tokens[1], 10, 0, 0xFF, -1);
        if (v_min < 0)
            return FALSE;
    }
    if (n_tokens >= 3) {
        v_mic = _nm_utils_ascii_str_to_int64(s_ver_tokens[2], 10, 0, 0xFF, -1);
        if (v_mic < 0)
            return FALSE;
    }

    nm_decode_version(cur_nm_version, &c_maj, &c_min, &c_mic);

#define CHECK_AND_RETURN_FALSE(cur, val, tag, is_last_digit)        \
    G_STMT_START                                                    \
    {                                                               \
        if (!strcmp(tag, MATCH_TAG_CONFIG_NM_VERSION_MIN)) {        \
            if (cur < val)                                          \
                return FALSE;                                       \
        } else if (!strcmp(tag, MATCH_TAG_CONFIG_NM_VERSION_MAX)) { \
            if (cur > val)                                          \
                return FALSE;                                       \
        } else {                                                    \
            if (cur != val)                                         \
                return FALSE;                                       \
        }                                                           \
        if (!(is_last_digit)) {                                     \
            if (cur != val)                                         \
                return FALSE;                                       \
        }                                                           \
    }                                                               \
    G_STMT_END
    if (v_mic >= 0)
        CHECK_AND_RETURN_FALSE(c_mic, v_mic, tag, TRUE);
    if (v_min >= 0)
        CHECK_AND_RETURN_FALSE(c_min, v_min, tag, v_mic < 0);
    CHECK_AND_RETURN_FALSE(c_maj, v_maj, tag, v_min < 0);
    return TRUE;
}