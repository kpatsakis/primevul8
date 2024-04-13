nm_utils_array_remove_at_indexes(GArray *array, const guint *indexes_to_delete, gsize len)
{
    gsize elt_size;
    guint index_to_delete;
    guint i_src;
    guint mm_src, mm_dst, mm_len;
    gsize i_itd;
    guint res_length;

    g_return_if_fail(array);
    if (!len)
        return;
    g_return_if_fail(indexes_to_delete);

    elt_size = g_array_get_element_size(array);

    i_itd           = 0;
    index_to_delete = indexes_to_delete[0];
    if (index_to_delete >= array->len)
        g_return_if_reached();

    res_length = array->len - 1;

    mm_dst = index_to_delete;
    mm_src = index_to_delete;
    mm_len = 0;

    for (i_src = index_to_delete; i_src < array->len; i_src++) {
        if (i_src < index_to_delete)
            mm_len++;
        else {
            /* we require indexes_to_delete to contain non-repeated, ascending
             * indexes. Otherwise, we would need to presort the indexes. */
            while (TRUE) {
                guint dd;

                if (i_itd + 1 >= len) {
                    index_to_delete = G_MAXUINT;
                    break;
                }

                dd = indexes_to_delete[++i_itd];
                if (dd > index_to_delete) {
                    if (dd >= array->len)
                        g_warn_if_reached();
                    else {
                        g_assert(res_length > 0);
                        res_length--;
                    }
                    index_to_delete = dd;
                    break;
                }
                g_warn_if_reached();
            }

            if (mm_len) {
                memmove(&array->data[mm_dst * elt_size],
                        &array->data[mm_src * elt_size],
                        mm_len * elt_size);
                mm_dst += mm_len;
                mm_src += mm_len + 1;
                mm_len = 0;
            } else
                mm_src++;
        }
    }
    if (mm_len) {
        memmove(&array->data[mm_dst * elt_size],
                &array->data[mm_src * elt_size],
                mm_len * elt_size);
    }
    g_array_set_size(array, res_length);
}