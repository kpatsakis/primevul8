gen_session_key(kdc_realm_t *kdc_active_realm, krb5_kdc_req *req,
                krb5_db_entry *server, krb5_keyblock *skey,
                const char **status)
{
    krb5_error_code retval;
    krb5_enctype useenctype = 0;

    /*
     * Some special care needs to be taken in the user-to-user
     * case, since we don't know what keytypes the application server
     * which is doing user-to-user authentication can support.  We
     * know that it at least must be able to support the encryption
     * type of the session key in the TGT, since otherwise it won't be
     * able to decrypt the U2U ticket!  So we use that in preference
     * to anything else.
     */
    if (req->kdc_options & KDC_OPT_ENC_TKT_IN_SKEY) {
        retval = get_2ndtkt_enctype(kdc_active_realm, req, &useenctype,
                                    status);
        if (retval != 0)
            return retval;
    }
    if (useenctype == 0) {
        useenctype = select_session_keytype(kdc_active_realm, server,
                                            req->nktypes,
                                            req->ktype);
    }
    if (useenctype == 0) {
        /* unsupported ktype */
        *status = "BAD_ENCRYPTION_TYPE";
        return KRB5KDC_ERR_ETYPE_NOSUPP;
    }

    return krb5_c_make_random_key(kdc_context, useenctype, skey);
}