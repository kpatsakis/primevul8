static krb5_error_code verify_krb5_user(krb5_context context, krb5_principal principal, const char *password, krb5_principal server, unsigned char verify)
{
    krb5_creds creds;
    krb5_get_init_creds_opt *gic_options;
    krb5_verify_init_creds_opt vic_options;
    krb5_error_code ret;
    char *name = NULL;

    memset(&creds, 0, sizeof(creds));

    // verify passed in client principal
    ret = krb5_unparse_name(context, principal, &name);
    if (ret == 0) {
#ifdef PRINTFS
        printf("Trying to get TGT for user %s\n", name);
#endif
        free(name);
    }

    // verify passed in server principal if needed
    if (verify) {
        ret = krb5_unparse_name(context, server, &name);
        if (ret == 0) {
#ifdef PRINTFS
            printf("Trying to get TGT for service %s\n", name);
#endif
            free(name);
        }
    }

    // verify password
    krb5_get_init_creds_opt_alloc(context, &gic_options);
    ret = krb5_get_init_creds_password(context, &creds, principal, (char *)password, NULL, NULL, 0, NULL, gic_options);
    if (ret) {
        set_basicauth_error(context, ret);
        goto end;
    }

    // verify response authenticity
    if (verify) {
        krb5_verify_init_creds_opt_init(&vic_options);
        krb5_verify_init_creds_opt_set_ap_req_nofail(&vic_options, 1);
        ret = krb5_verify_init_creds(context, &creds, server, NULL, NULL, &vic_options);
        if (ret) {
            set_basicauth_error(context, ret);
        }
    }

end:
    // clean up
    krb5_free_cred_contents(context, &creds);

    if (gic_options)
        krb5_get_init_creds_opt_free(context, gic_options);

    return ret;
}