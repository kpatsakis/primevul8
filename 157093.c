static void FNAME_DECL(uncompress_row)(const PIXEL * const prev_row,
                                       PIXEL * const cur_row,
                                       unsigned int width)

{
    DECLARE_STATE_VARIABLES;
    const unsigned int bpc_mask = BPC_MASK;
    unsigned int pos = 0;

    while ((DEFwmimax > (int)state->wmidx) && (state->wmileft <= width)) {
        if (state->wmileft) {
            FNAME_CALL(uncompress_row_seg)(prev_row, cur_row, pos,
                                           pos + state->wmileft, bpc_mask);
            pos += state->wmileft;
            width -= state->wmileft;
        }

        state->wmidx++;
        set_wm_trigger(state);
        state->wmileft = DEFwminext;
    }

    if (width) {
        FNAME_CALL(uncompress_row_seg)(prev_row, cur_row, pos,
                                       pos + width, bpc_mask);
        if (DEFwmimax > (int)state->wmidx) {
            state->wmileft -= width;
        }
    }

    spice_assert((int)state->wmidx <= DEFwmimax);
    spice_assert(state->wmidx <= 32);
    spice_assert(DEFwminext > 0);
}