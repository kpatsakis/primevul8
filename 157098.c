static void FNAME_DECL(compress_row_seg)(int i,
                                         const PIXEL * const prev_row,
                                         const PIXEL * const cur_row,
                                         const int end,
                                         const unsigned int waitmask,
                                         const unsigned int bpc_mask)
{
    DECLARE_STATE_VARIABLES;
    DECLARE_CHANNEL_VARIABLES;
    int stopidx;
    int run_index = 0;
    int run_size;

    spice_assert(end - i > 0);

    if (i == 0) {
        APPLY_ALL_COMP(COMPRESS_ONE_0);

        if (state->waitcnt) {
            state->waitcnt--;
        } else {
            state->waitcnt = (tabrand(&state->tabrand_seed) & waitmask);
            UPDATE_MODEL(0);
        }
        stopidx = ++i + state->waitcnt;
    } else {
        stopidx = i + state->waitcnt;
    }
    for (;;) {
        while (stopidx < end) {
            for (; i <= stopidx; i++) {
                RLE_PRED_IMP;
                APPLY_ALL_COMP(COMPRESS_ONE, i);
            }

            UPDATE_MODEL(stopidx);
            stopidx = i + (tabrand(&state->tabrand_seed) & waitmask);
        }

        for (; i < end; i++) {
            RLE_PRED_IMP;
            APPLY_ALL_COMP(COMPRESS_ONE, i);
        }
        state->waitcnt = stopidx - end;

        return;

do_run:
        run_index = i;
        state->waitcnt = stopidx - i;
        run_size = 0;

        while (SAME_PIXEL(&cur_row[i], &cur_row[i - 1])) {
            run_size++;
            if (++i == end) {
                encode_state_run(encoder, state, run_size);
                return;
            }
        }
        encode_state_run(encoder, state, run_size);
        stopidx = i + state->waitcnt;
    }
}