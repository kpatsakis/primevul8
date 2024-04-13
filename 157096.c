static void FNAME_DECL(compress_row)(const PIXEL * const prev_row,
                                     const PIXEL * const cur_row,
                                     unsigned int width)

{
    DECLARE_STATE_VARIABLES;
    const unsigned int bpc_mask = BPC_MASK;
    unsigned int pos = 0;

    while ((DEFwmimax > (int)state->wmidx) && (state->wmileft <= width)) {
        if (state->wmileft) {
            FNAME_CALL(compress_row_seg)(pos, prev_row, cur_row,
                                         pos + state->wmileft, bppmask[state->wmidx],
                                         bpc_mask);
            width -= state->wmileft;
            pos += state->wmileft;
        }

        state->wmidx++;
        set_wm_trigger(state);
        state->wmileft = DEFwminext;
    }

    if (width) {
        FNAME_CALL(compress_row_seg)(pos, prev_row, cur_row, pos + width,
                                     bppmask[state->wmidx], bpc_mask);
        if (DEFwmimax > (int)state->wmidx) {
            state->wmileft -= width;
        }
    }

    spice_assert((int)state->wmidx <= DEFwmimax);
    spice_assert(state->wmidx <= 32);
    spice_assert(DEFwminext > 0);
}