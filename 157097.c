static void FNAME_DECL(uncompress_row0_seg)(int i,
                                            PIXEL * const cur_row,
                                            const int end,
                                            const unsigned int waitmask,
                                            const unsigned int bpc_mask)
{
    DECLARE_STATE_VARIABLES;
    DECLARE_CHANNEL_VARIABLES;
    int stopidx;

    spice_assert(end - i > 0);

    if (i == 0) {
        unsigned int codewordlen;

        UNCOMPRESS_PIX_START(&cur_row[i]);
        APPLY_ALL_COMP(UNCOMPRESS_ONE_ROW0_0);

        if (state->waitcnt) {
            --state->waitcnt;
        } else {
            state->waitcnt = (tabrand(&state->tabrand_seed) & waitmask);
            UPDATE_MODEL(0);
        }
        stopidx = ++i + state->waitcnt;
    } else {
        stopidx = i + state->waitcnt;
    }

    while (stopidx < end) {
        for (; i <= stopidx; i++) {
            unsigned int codewordlen;

            UNCOMPRESS_PIX_START(&cur_row[i]);
            APPLY_ALL_COMP(UNCOMPRESS_ONE_ROW0);
        }
        UPDATE_MODEL(stopidx);
        stopidx = i + (tabrand(&state->tabrand_seed) & waitmask);
    }

    for (; i < end; i++) {
        unsigned int codewordlen;

        UNCOMPRESS_PIX_START(&cur_row[i]);
        APPLY_ALL_COMP(UNCOMPRESS_ONE_ROW0);
    }
    state->waitcnt = stopidx - end;
}