db_get_svc_princ(krb5_context ctx, krb5_principal princ,
                 krb5_flags flags, krb5_db_entry **server,
                 const char **status)
{
    krb5_error_code ret;

    ret = krb5_db_get_principal(ctx, princ, flags, server);
    if (ret == KRB5_KDB_CANTLOCK_DB)
        ret = KRB5KDC_ERR_SVC_UNAVAILABLE;
    if (ret != 0) {
        *status = "LOOKING_UP_SERVER";
    }
    return ret;
}