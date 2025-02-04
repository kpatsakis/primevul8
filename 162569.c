process_body_part(proto_tree *tree, tvbuff_t *tvb,
        http_message_info_t *input_message_info, multipart_info_t *m_info,
        packet_info *pinfo, gint start, gint idx,
        gboolean *last_boundary)
{
    proto_tree *subtree;
    proto_item *ti;
    gint offset = start, next_offset = 0;
    http_message_info_t message_info = { input_message_info->type, NULL };
    gint body_start, boundary_start, boundary_line_len;

    gchar *content_type_str = NULL;
    gchar *content_encoding_str = NULL;
    char *filename = NULL;
    char *mimetypename = NULL;
    gboolean last_field = FALSE;
    gboolean is_raw_data = FALSE;

    const guint8 *boundary = (guint8 *)m_info->boundary;
    gint boundary_len = m_info->boundary_length;

    ti = proto_tree_add_item(tree, hf_multipart_part, tvb, start, 0, ENC_ASCII|ENC_NA);
    subtree = proto_item_add_subtree(ti, ett_multipart_body);

    /* find the next boundary to find the end of this body part */
    boundary_start = find_next_boundary(tvb, offset, boundary, boundary_len,
            &boundary_line_len, last_boundary);

    if (boundary_start <= 0) {
        return -1;
    }

    /*
     * Process the MIME-part-headers
     */

    while (!last_field)
    {
        gint colon_offset;
        char *hdr_str;
        char *header_str;

        /* Look for the end of the header (denoted by cr)
         * 3:d argument to imf_find_field_end() maxlen; must be last offset in the tvb.
         */
        next_offset = imf_find_field_end(tvb, offset, tvb_reported_length_remaining(tvb, offset)+offset, &last_field);
        /* the following should never happen */
        /* If cr not found, won't have advanced - get out to avoid infinite loop! */
        /*
        if (next_offset == offset) {
            break;
        }
        */
        if (last_field && (next_offset+2) <= boundary_start) {
            /* Add the extra CRLF of the last field */
            next_offset += 2;
        } else if((next_offset-2) == boundary_start) {
            /* if CRLF is the start of next boundary it belongs to the boundary and not the field,
               so it's the last field without CRLF */
            last_field = TRUE;
            next_offset -= 2;
        } else if (next_offset > boundary_start) {
            /* if there is no CRLF between last field and next boundary - trim it! */
            next_offset = boundary_start;
        }

        hdr_str = tvb_get_string_enc(wmem_packet_scope(), tvb, offset, next_offset - offset, ENC_ASCII);

        colon_offset = 0;
        header_str = unfold_and_compact_mime_header(hdr_str, &colon_offset);
        if (colon_offset <= 0) {
            /* if there is no colon it's no header, so break and add complete line to the body */
            next_offset = offset;
            break;
        } else {
            gint hf_index;

            hf_index = is_known_multipart_header(header_str, colon_offset);

            if (hf_index == -1) {
                if(isprint_string(header_str)) {
                    proto_tree_add_format_text(subtree, tvb, offset, next_offset - offset);
                } else {
                    /* if the header name is unkown and not printable, break and add complete line to the body */
                    next_offset = offset;
                    break;
                }
            } else {
                char *value_str = wmem_strdup(wmem_packet_scope(), header_str + colon_offset + 1);

                proto_tree_add_string_format(subtree,
                      hf_header_array[hf_index], tvb,
                      offset, next_offset - offset,
                      (const char *)value_str, "%s",
                      tvb_format_text(tvb, offset, next_offset - offset));

                switch (hf_index) {
                    case POS_ORIGINALCONTENT:
                        {
                            char *semicolonp;
                            /* The Content-Type starts at colon_offset + 1 or after the type parameter */
                            char* type_str = ws_find_media_type_parameter(wmem_packet_scope(), value_str, "type");
                            if(type_str != NULL) {
                                value_str = type_str;
                            }

                            semicolonp = strchr(value_str, ';');

                            if (semicolonp != NULL) {
                                *semicolonp = '\0';
                                m_info->orig_parameters = wmem_strdup(wmem_packet_scope(),
                                                             semicolonp + 1);
                            }

                            m_info->orig_content_type = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);
                        }
                        break;
                    case POS_CONTENT_TYPE:
                        {
                            /* The Content-Type starts at colon_offset + 1 */
                            char *semicolonp = strchr(value_str, ';');

                            if (semicolonp != NULL) {
                                *semicolonp = '\0';
                                message_info.media_str = wmem_strdup(wmem_packet_scope(), semicolonp + 1);
                            } else {
                                message_info.media_str = NULL;
                            }

                            content_type_str = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);

                            /* Show content-type in root 'part' label */
                            proto_item_append_text(ti, " (%s)", content_type_str);

                            /* find the "name" parameter in case we don't find a content disposition "filename" */
                            mimetypename = ws_find_media_type_parameter(wmem_packet_scope(), message_info.media_str, "name");

                            if(strncmp(content_type_str, "application/octet-stream",
                                    sizeof("application/octet-stream")-1) == 0) {
                                is_raw_data = TRUE;
                            }

                            /* there are only 2 body parts possible and each part has specific content types */
                            if(m_info->protocol && idx == 0
                                && (is_raw_data || g_ascii_strncasecmp(content_type_str, m_info->protocol,
                                                        strlen(m_info->protocol)) != 0))
                            {
                                return -1;
                            }
                        }
                        break;
                    case POS_CONTENT_TRANSFER_ENCODING:
                        {
                            /* The Content-Transferring starts at colon_offset + 1 */
                            char *crp = strchr(value_str, '\r');

                            if (crp != NULL) {
                                *crp = '\0';
                            }

                            content_encoding_str = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);
                        }
                        break;
                    case POS_CONTENT_DISPOSITION:
                        {
                            /* find the "filename" parameter */
                            filename = ws_find_media_type_parameter(wmem_packet_scope(), value_str, "filename");
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        offset = next_offset;
    }

    body_start = next_offset;

    /*
     * Process the body
     */

    {
        gint body_len = boundary_start - body_start;
        tvbuff_t *tmp_tvb = tvb_new_subset_length(tvb, body_start, body_len);
        /* if multipart subtype is encrypted the protcol string was set */
        /* see: https://msdn.microsoft.com/en-us/library/cc251581.aspx */
        /* there are only 2 body parts possible and each part has specific content types */
        if(m_info->protocol && idx == 1 && is_raw_data)
        {
            gssapi_encrypt_info_t  encrypt;

            memset(&encrypt, 0, sizeof(encrypt));
            encrypt.decrypt_gssapi_tvb=DECRYPT_GSSAPI_NORMAL;

            dissect_kerberos_encrypted_message(tmp_tvb, pinfo, subtree, &encrypt);

            if(encrypt.gssapi_decrypted_tvb){
                    tmp_tvb = encrypt.gssapi_decrypted_tvb;
                    is_raw_data = FALSE;
                    content_type_str = m_info->orig_content_type;
                    message_info.media_str = m_info->orig_parameters;
            } else if(encrypt.gssapi_encrypted_tvb) {
                    tmp_tvb = encrypt.gssapi_encrypted_tvb;
                    proto_tree_add_expert(tree, pinfo, &ei_multipart_decryption_not_possible, tmp_tvb, 0, -1);
            }
        }

        if (!is_raw_data &&
            content_type_str) {

            /*
             * subdissection
             */
            gboolean dissected;

            /*
             * Try and remove any content transfer encoding so that each sub-dissector
             * doesn't have to do it itself
             *
             */

            if(content_encoding_str && remove_base64_encoding) {

                if(!g_ascii_strncasecmp(content_encoding_str, "base64", 6))
                    tmp_tvb = base64_decode(pinfo, tmp_tvb, filename ? filename : (mimetypename ? mimetypename : content_type_str));

            }

            /*
             * First try the dedicated multipart dissector table
             */
            dissected = dissector_try_string(multipart_media_subdissector_table,
                        content_type_str, tmp_tvb, pinfo, subtree, &message_info);
            if (! dissected) {
                /*
                 * Fall back to the default media dissector table
                 */
                dissected = dissector_try_string(media_type_dissector_table,
                        content_type_str, tmp_tvb, pinfo, subtree, &message_info);
            }
            if (! dissected) {
                const char *save_match_string = pinfo->match_string;
                pinfo->match_string = content_type_str;
                call_dissector_with_data(media_handle, tmp_tvb, pinfo, subtree, &message_info);
                pinfo->match_string = save_match_string;
            }
            message_info.media_str = NULL; /* Shares same memory as content_type_str */
        } else {
            call_data_dissector(tmp_tvb, pinfo, subtree);
        }
        proto_item_set_len(ti, boundary_start - start);
        if (*last_boundary == TRUE) {
           proto_tree_add_item(tree, hf_multipart_last_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
        } else {
           proto_tree_add_item(tree, hf_multipart_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
        }

        return boundary_start + boundary_line_len;
    }
}