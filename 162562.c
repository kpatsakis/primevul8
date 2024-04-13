proto_register_multipart(void)
{

/* Setup list of header fields  See Section 1.6.1 for details */
    static hf_register_info hf[] = {
        { &hf_multipart_type,
          {   "Type",
              "mime_multipart.type",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "MIME multipart encapsulation type", HFILL
          }
        },
        { &hf_multipart_part,
          {   "Encapsulated multipart part",
              "mime_multipart.part",
              FT_STRING, BASE_NONE, NULL, 0x00,
              NULL, HFILL
          }
        },
        { &hf_multipart_sec_token_len,
          {   "Length of security token",
              "mime_multipart.header.sectoken-length",
              FT_UINT32, BASE_DEC, NULL, 0x00,
              "Length of the Kerberos BLOB which follows this token", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_DESCRIPTION],
          {   "Content-Description",
              "mime_multipart.header.content-description",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "Content-Description Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_DISPOSITION],
          {   "Content-Disposition",
              "mime_multipart.header.content-disposition",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "RFC 2183: Content-Disposition Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_ENCODING],
          {   "Content-Encoding",
              "mime_multipart.header.content-encoding",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "Content-Encoding Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_ID],
          {   "Content-Id",
              "mime_multipart.header.content-id",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "RFC 2045: Content-Id Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_LANGUAGE],
          {   "Content-Language",
              "mime_multipart.header.content-language",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "Content-Language Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_LENGTH],
          {   "Content-Length",
              "mime_multipart.header.content-length",
              FT_STRING, BASE_NONE, NULL, 0x0,
              "Content-Length Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_TRANSFER_ENCODING],
          {   "Content-Transfer-Encoding",
              "mime_multipart.header.content-transfer-encoding",
              FT_STRING, BASE_NONE, NULL, 0x00,
              "RFC 2045: Content-Transfer-Encoding Header", HFILL
          }
        },
        { &hf_header_array[POS_CONTENT_TYPE],
          {   "Content-Type",
              "mime_multipart.header.content-type",
              FT_STRING, BASE_NONE,NULL,0x0,
              "Content-Type Header", HFILL
          }
        },
        { &hf_header_array[POS_ORIGINALCONTENT],
          {   "OriginalContent",
              "mime_multipart.header.originalcontent",
              FT_STRING, BASE_NONE,NULL,0x0,
              "Original Content-Type Header", HFILL
          }
        },

      /* Generated from convert_proto_tree_add_text.pl */
      { &hf_multipart_first_boundary, { "First boundary", "mime_multipart.first_boundary", FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }},
      { &hf_multipart_preamble, { "Preamble", "mime_multipart.preamble", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},
      { &hf_multipart_last_boundary, { "Last boundary", "mime_multipart.last_boundary", FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }},
      { &hf_multipart_boundary, { "Boundary", "mime_multipart.boundary", FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }},
      { &hf_multipart_trailer, { "Trailer", "mime_multipart.trailer", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},

    };

    /*
     * Preferences
     */
    module_t *multipart_module;
    expert_module_t* expert_multipart;


    /*
     * Setup protocol subtree array
     */
    static gint *ett[] = {
        &ett_multipart,
        &ett_multipart_main,
        &ett_multipart_body,
    };

    static ei_register_info ei[] = {
        { &ei_multipart_no_required_parameter, { "mime_multipart.no_required_parameter", PI_PROTOCOL, PI_ERROR, "The multipart dissector could not find a required parameter.", EXPFILL }},
        { &ei_multipart_decryption_not_possible, { "mime_multipart.decryption_not_possible", PI_UNDECODED, PI_WARN, "The multipart dissector could not decrypt the message.", EXPFILL }},
    };

    /*
     * Register the protocol name and description
     */
    proto_multipart = proto_register_protocol(
        "MIME Multipart Media Encapsulation",
        "MIME multipart",
        "mime_multipart");

    /*
     * Required function calls to register
     * the header fields and subtrees used.
     */
    proto_register_field_array(proto_multipart, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
    expert_multipart = expert_register_protocol(proto_multipart);
    expert_register_field_array(expert_multipart, ei, array_length(ei));

    multipart_module = prefs_register_protocol(proto_multipart, NULL);

    prefs_register_bool_preference(multipart_module,
                                   "display_unknown_body_as_text",
                                   "Display bodies without media type as text",
                                   "Display multipart bodies with no media type dissector"
                                   " as raw text (may cause problems with binary data).",
                                   &display_unknown_body_as_text);

    prefs_register_bool_preference(multipart_module,
                                   "remove_base64_encoding",
                                   "Remove base64 encoding from bodies",
                                   "Remove any base64 content-transfer encoding from bodies. "
                                   "This supports export of the body and its further dissection.",
                                   &remove_base64_encoding);

    /*
     * Dissectors requiring different behavior in cases where the media
     * is contained in a multipart entity should register their multipart
     * dissector in the dissector table below, which is similar to the
     * "media_type" dissector table defined in the HTTP dissector code.
     */
    multipart_media_subdissector_table = register_dissector_table(
        "multipart_media_type",
        "Internet media type (for multipart processing)",
        proto_multipart, FT_STRING, BASE_NONE);
}