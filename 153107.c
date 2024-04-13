get_2ndtkt_enctype(kdc_realm_t *kdc_active_realm, krb5_kdc_req *req,
                   krb5_enctype *useenctype, const char **status)
{
    krb5_enctype etype;
    krb5_ticket *stkt = req->second_ticket[0];
    int i;

    etype = stkt->enc_part2->session->enctype;
    if (!krb5_c_valid_enctype(etype)) {
        *status = "BAD_ETYPE_IN_2ND_TKT";
        return KRB5KDC_ERR_ETYPE_NOSUPP;
    }
    for (i = 0; i < req->nktypes; i++) {
        if (req->ktype[i] == etype) {
            *useenctype = etype;
            break;
        }
    }
    return 0;
}