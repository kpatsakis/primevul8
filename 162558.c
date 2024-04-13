proto_reg_handoff_multipart(void)
{
    dissector_handle_t multipart_handle;

    /*
     * When we cannot display the data, call the data dissector.
     * When there is no dissector for the given media, call the media dissector.
     */
    media_handle = find_dissector_add_dependency("media", proto_multipart);
    gssapi_handle = find_dissector_add_dependency("gssapi", proto_multipart);

    /*
     * Get the content type and Internet media type table
     */
    media_type_dissector_table = find_dissector_table("media_type");

    /*
     * Handle for multipart dissection
     */
    multipart_handle = create_dissector_handle(
            dissect_multipart, proto_multipart);

    dissector_add_string("media_type",
            "multipart/mixed", multipart_handle);
    dissector_add_string("media_type",
            "multipart/related", multipart_handle);
    dissector_add_string("media_type",
            "multipart/alternative", multipart_handle);
    dissector_add_string("media_type",
            "multipart/form-data", multipart_handle);
    dissector_add_string("media_type",
            "multipart/report", multipart_handle);
    dissector_add_string("media_type",
            "multipart/signed", multipart_handle);
    dissector_add_string("media_type",
            "multipart/encrypted", multipart_handle);

    /*
     * Supply an entry to use for unknown multipart subtype.
     * See RFC 2046, section 5.1.3
     */
    dissector_add_string("media_type",
            "multipart/", multipart_handle);
}