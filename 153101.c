search_sprinc(kdc_realm_t *kdc_active_realm, krb5_kdc_req *req,
              krb5_flags flags, krb5_db_entry **server, const char **status)
{
    krb5_error_code ret;
    krb5_principal princ = req->server;
    krb5_principal reftgs = NULL;
    krb5_boolean allow_referral;

    /* Do not allow referrals for u2u or ticket modification requests, because
     * the server is supposed to match an already-issued ticket. */
    allow_referral = !(req->kdc_options & NO_REFERRAL_OPTION);
    if (!allow_referral)
        flags &= ~KRB5_KDB_FLAG_CANONICALIZE;

    ret = db_get_svc_princ(kdc_context, princ, flags, server, status);
    if (ret == 0 || ret != KRB5_KDB_NOENTRY || !allow_referral)
        goto cleanup;

    if (!is_cross_tgs_principal(req->server)) {
        ret = find_referral_tgs(kdc_active_realm, req, &reftgs);
        if (ret != 0)
            goto cleanup;
        ret = db_get_svc_princ(kdc_context, reftgs, flags, server, status);
        if (ret == 0 || ret != KRB5_KDB_NOENTRY)
            goto cleanup;

        princ = reftgs;
    }
    ret = find_alternate_tgs(kdc_active_realm, princ, server, status);

cleanup:
    if (ret != 0 && ret != KRB5KDC_ERR_SVC_UNAVAILABLE) {
        ret = KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN;
        if (*status == NULL)
            *status = "LOOKING_UP_SERVER";
    }
    krb5_free_principal(kdc_context, reftgs);
    return ret;
}