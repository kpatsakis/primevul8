ngx_http_clean_header(ngx_http_request_t *r)
{
    ngx_memzero(&r->headers_out.status,
                sizeof(ngx_http_headers_out_t)
                    - offsetof(ngx_http_headers_out_t, status));

    r->headers_out.headers.part.nelts = 0;
    r->headers_out.headers.part.next = NULL;
    r->headers_out.headers.last = &r->headers_out.headers.part;

    r->headers_out.content_length_n = -1;
    r->headers_out.last_modified_time = -1;
}