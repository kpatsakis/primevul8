match_device_eval(const char *spec_str, gboolean allow_fuzzy, MatchDeviceData *match_data)
{
    if (spec_str[0] == '*' && spec_str[1] == '\0')
        return TRUE;

    if (_MATCH_CHECK(spec_str, DEVICE_TYPE_TAG)) {
        return match_data->device_type && nm_streq(spec_str, match_data->device_type);
    }

    if (_MATCH_CHECK(spec_str, NM_MATCH_SPEC_MAC_TAG))
        return match_device_hwaddr_eval(spec_str, match_data);

    if (_MATCH_CHECK(spec_str, NM_MATCH_SPEC_INTERFACE_NAME_TAG)) {
        gboolean use_pattern = FALSE;

        if (spec_str[0] == '=')
            spec_str += 1;
        else {
            if (spec_str[0] == '~')
                spec_str += 1;
            use_pattern = TRUE;
        }

        if (match_data->interface_name) {
            if (nm_streq(spec_str, match_data->interface_name))
                return TRUE;
            if (use_pattern && g_pattern_match_simple(spec_str, match_data->interface_name))
                return TRUE;
        }
        return FALSE;
    }

    if (_MATCH_CHECK(spec_str, DRIVER_TAG)) {
        const char *t;

        if (!match_data->driver)
            return FALSE;

        /* support:
         * 1) "${DRIVER}"
         *   In this case, DRIVER may not contain a '/' character.
         *   It matches any driver version.
         * 2) "${DRIVER}/${DRIVER_VERSION}"
         *   In this case, DRIVER may contains '/' but DRIVER_VERSION
         *   may not. A '/' in DRIVER_VERSION may be replaced by '?'.
         *
         * It follows, that "${DRIVER}/""*" is like 1), but allows
         * '/' inside DRIVER.
         *
         * The fields match to what `nmcli -f GENERAL.DRIVER,GENERAL.DRIVER-VERSION device show`
         * gives. However, DRIVER matches literally, while DRIVER_VERSION is a glob
         * supporting ? and *.
         */

        t = strrchr(spec_str, '/');

        if (!t)
            return nm_streq(spec_str, match_data->driver);

        return (strncmp(spec_str, match_data->driver, t - spec_str) == 0)
               && g_pattern_match_simple(&t[1], match_data->driver_version ?: "");
    }

    if (_MATCH_CHECK(spec_str, NM_MATCH_SPEC_S390_SUBCHANNELS_TAG))
        return match_data_s390_subchannels_eval(spec_str, match_data);

    if (_MATCH_CHECK(spec_str, DHCP_PLUGIN_TAG))
        return nm_streq0(spec_str, match_data->dhcp_plugin);

    if (allow_fuzzy) {
        if (match_device_hwaddr_eval(spec_str, match_data))
            return TRUE;
        if (match_data->interface_name && nm_streq(spec_str, match_data->interface_name))
            return TRUE;
    }

    return FALSE;
}