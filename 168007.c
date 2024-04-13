nm_match_spec_device(const GSList *specs,
                     const char *  interface_name,
                     const char *  device_type,
                     const char *  driver,
                     const char *  driver_version,
                     const char *  hwaddr,
                     const char *  s390_subchannels,
                     const char *  dhcp_plugin)
{
    const GSList *  iter;
    gboolean        has_match        = FALSE;
    gboolean        has_match_except = FALSE;
    gboolean        has_except       = FALSE;
    gboolean        has_not_except   = FALSE;
    const char *    spec_str;
    MatchDeviceData match_data = {
        .interface_name = interface_name,
        .device_type    = nm_str_not_empty(device_type),
        .driver         = nm_str_not_empty(driver),
        .driver_version = nm_str_not_empty(driver_version),
        .dhcp_plugin    = nm_str_not_empty(dhcp_plugin),
        .hwaddr =
            {
                .value = hwaddr,
            },
        .s390_subchannels =
            {
                .value = s390_subchannels,
            },
    };

    nm_assert(!hwaddr || nm_utils_hwaddr_valid(hwaddr, -1));

    if (!specs)
        return NM_MATCH_SPEC_NO_MATCH;

    for (iter = specs; iter; iter = iter->next) {
        gboolean except;

        spec_str = iter->data;

        if (!spec_str || !*spec_str)
            continue;

        spec_str = match_except(spec_str, &except);

        if (except)
            has_except = TRUE;
        else
            has_not_except = TRUE;

        if ((except && has_match_except) || (!except && has_match)) {
            /* evaluating the match does not give new information. Skip it. */
            continue;
        }

        if (!match_device_eval(spec_str, !except, &match_data))
            continue;

        if (except)
            has_match_except = TRUE;
        else
            has_match = TRUE;
    }

    return _match_result(has_except, has_not_except, has_match, has_match_except);
}