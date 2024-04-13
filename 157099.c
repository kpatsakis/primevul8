static void FNAME_DECL(uncompress_row0)(PIXEL * const cur_row,
                                        unsigned int width)

{
    DECLARE_STATE_VARIABLES;
    const unsigned int bpc_mask = BPC_MASK;
    unsigned int pos = 0;

    while ((DEFwmimax > (int)state->wmidx) && (state->wmileft <= width)) {
        if (state->wmileft) {
            FNAME_CALL(uncompress_row0_seg)(pos, cur_row,
                                            pos + state->wmileft,
                                            bppmask[state->wmidx],
                                            bpc_mask);
            pos += state->wmileft;
            width -= state->wmileft;
        }

        state->wmidx++;
        set_wm_trigger(state);
        state->wmileft = DEFwminext;
    }

    if (width) {
        FNAME_CALL(uncompress_row0_seg)(pos, cur_row, pos + width,
                                        bppmask[state->wmidx], bpc_mask);
        if (DEFwmimax > (int)state->wmidx) {
            state->wmileft -= width;
        }
    }

    spice_assert((int)state->wmidx <= DEFwmimax);
    spice_assert(state->wmidx <= 32);
    spice_assert(DEFwminext > 0);
}