find_next_boundary(tvbuff_t *tvb, gint start, const guint8 *boundary,
        gint boundary_len, gint *boundary_line_len, gboolean *last_boundary)
{
    gint offset = start, next_offset, line_len, boundary_start;

    while (tvb_offset_exists(tvb, offset + 2 + boundary_len)) {
        line_len =  tvb_find_line_end(tvb, offset, -1, &next_offset, FALSE);
        if (line_len == -1) {
            return -1;
        }
        boundary_start = offset + line_len;
        if (((tvb_strneql(tvb, next_offset, (const guint8 *)"--", 2) == 0)
                    && (tvb_strneql(tvb, next_offset + 2, boundary, boundary_len) == 0)))
        {
            /* Boundary string; now check if last */
            if ((tvb_reported_length_remaining(tvb, next_offset + 2 + boundary_len + 2) >= 0)
                    && (tvb_strneql(tvb, next_offset + 2 + boundary_len,
                            (const guint8 *)"--", 2) == 0)) {
                *last_boundary = TRUE;
            } else {
                *last_boundary = FALSE;
            }
            /* Look for line end of the boundary line */
            line_len =  tvb_find_line_end(tvb, next_offset, -1, &offset, FALSE);
            if (line_len == -1) {
                *boundary_line_len = -1;
            } else {
                *boundary_line_len = offset - boundary_start;
            }
            return boundary_start;
        /* check if last before CRLF; some ignore the standard, so there is no CRLF before the boundary */
        } else if ((tvb_strneql(tvb, boundary_start - 2, (const guint8 *)"--", 2) == 0)
                    && (tvb_strneql(tvb, boundary_start - (2 + boundary_len), boundary, boundary_len) == 0)
                    && (tvb_strneql(tvb, boundary_start - (2 + boundary_len + 2),
                            (const guint8 *)"--", 2) == 0)) {
            boundary_start -= 2 + boundary_len + 2;
            *boundary_line_len = next_offset - boundary_start;
            *last_boundary = TRUE;
            return boundary_start;
        }
        offset = next_offset;
    }

    return -1;
}