iff_getop(uint8_t *list, int pos)
{
    uint8_t *item;
    uint8_t mask = 3, result;

    assert(pos >= 0);

    item = &list[pos / 4];
    result = (*item) & (mask << 2 * (pos % 4));
    return result >> 2 * (pos % 4);
}