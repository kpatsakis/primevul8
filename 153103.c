is_referral_req(kdc_realm_t *kdc_active_realm, krb5_kdc_req *request)
{
    krb5_boolean ret = FALSE;
    char *stype = NULL;
    char *hostbased = kdc_active_realm->realm_hostbased;
    char *no_referral = kdc_active_realm->realm_no_referral;

    if (!(request->kdc_options & KDC_OPT_CANONICALIZE))
        return FALSE;

    if (request->kdc_options & KDC_OPT_ENC_TKT_IN_SKEY)
        return FALSE;

    if (krb5_princ_size(kdc_context, request->server) != 2)
        return FALSE;

    stype = data2string(krb5_princ_component(kdc_context, request->server, 0));
    if (stype == NULL)
        return FALSE;
    switch (krb5_princ_type(kdc_context, request->server)) {
    case KRB5_NT_UNKNOWN:
        /* Allow referrals for NT-UNKNOWN principals, if configured. */
        if (!in_list(hostbased, stype) && !in_list(hostbased, "*"))
            goto cleanup;
        /* FALLTHROUGH */
    case KRB5_NT_SRV_HST:
    case KRB5_NT_SRV_INST:
        /* Deny referrals for specific service types, if configured. */
        if (in_list(no_referral, stype) || in_list(no_referral, "*"))
            goto cleanup;
        ret = TRUE;
        break;
    default:
        goto cleanup;
    }
cleanup:
    free(stype);
    return ret;
}