static int test_and_set_read_order_bit(ASS_Track *track, int id)
{
    if (resize_read_order_bitmap(track, id) < 0)
        return -1;
    int index = id / 32;
    uint32_t bit = 1u << (id % 32);
    if (track->parser_priv->read_order_bitmap[index] & bit)
        return 1;
    track->parser_priv->read_order_bitmap[index] |= bit;
    return 0;
}