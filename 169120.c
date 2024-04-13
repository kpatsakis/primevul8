static int resize_read_order_bitmap(ASS_Track *track, int max_id)
{
    // Don't allow malicious files to OOM us easily. Also avoids int overflows.
    if (max_id < 0 || max_id >= 10 * 1024 * 1024 * 8)
        goto fail;
    assert(track->parser_priv->read_order_bitmap || !track->parser_priv->read_order_elems);
    if (max_id >= track->parser_priv->read_order_elems * 32) {
        int oldelems = track->parser_priv->read_order_elems;
        int elems = ((max_id + 31) / 32 + 1) * 2;
        assert(elems >= oldelems);
        track->parser_priv->read_order_elems = elems;
        void *new_bitmap =
            realloc(track->parser_priv->read_order_bitmap, elems * 4);
        if (!new_bitmap)
            goto fail;
        track->parser_priv->read_order_bitmap = new_bitmap;
        memset(track->parser_priv->read_order_bitmap + oldelems, 0,
               (elems - oldelems) * 4);
    }
    return 0;

fail:
    free(track->parser_priv->read_order_bitmap);
    track->parser_priv->read_order_bitmap = NULL;
    track->parser_priv->read_order_elems = 0;
    return -1;
}