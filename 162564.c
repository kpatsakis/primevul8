process_preamble(proto_tree *tree, tvbuff_t *tvb, multipart_info_t *m_info,
        gboolean *last_boundary)
{
    gint boundary_start, boundary_line_len;

    const guint8 *boundary = (guint8 *)m_info->boundary;
    gint boundary_len = m_info->boundary_length;

    boundary_start = find_first_boundary(tvb, 0, boundary, boundary_len,
            &boundary_line_len, last_boundary);
    if (boundary_start == 0) {
       proto_tree_add_item(tree, hf_multipart_first_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
        return boundary_start + boundary_line_len;
    } else if (boundary_start > 0) {
        if (boundary_line_len > 0) {
            gint body_part_start = boundary_start + boundary_line_len;

            if (boundary_start > 0) {
               proto_tree_add_item(tree, hf_multipart_preamble, tvb, 0, boundary_start, ENC_NA);
            }
            proto_tree_add_item(tree, hf_multipart_first_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
            return body_part_start;
        }
    }
    return -1;
}