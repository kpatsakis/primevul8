ngx_http_filter_finalize_request(ngx_http_request_t *r, ngx_module_t *m,
    ngx_int_t error)
{
    void       *ctx;
    ngx_int_t   rc;

    ngx_http_clean_header(r);

    ctx = NULL;

    if (m) {
        ctx = r->ctx[m->ctx_index];
    }

    /* clear the modules contexts */
    ngx_memzero(r->ctx, sizeof(void *) * ngx_http_max_module);

    if (m) {
        r->ctx[m->ctx_index] = ctx;
    }

    r->filter_finalize = 1;

    rc = ngx_http_special_response_handler(r, error);

    /* NGX_ERROR resets any pending data */

    switch (rc) {

    case NGX_OK:
    case NGX_DONE:
        return NGX_ERROR;

    default:
        return rc;
    }
}