ChangeStringFeedback(ClientPtr client, DeviceIntPtr dev,
                     long unsigned int mask, StringFeedbackPtr s,
                     xStringFeedbackCtl * f)
{
    int i, j;
    KeySym *syms, *sup_syms;

    syms = (KeySym *) (f + 1);
    if (client->swapped) {
        swaps(&f->length);      /* swapped num_keysyms in calling proc */
        SwapLongs((CARD32 *) syms, f->num_keysyms);
    }

    if (f->num_keysyms > s->ctrl.max_symbols)
        return BadValue;

    sup_syms = s->ctrl.symbols_supported;
    for (i = 0; i < f->num_keysyms; i++) {
        for (j = 0; j < s->ctrl.num_symbols_supported; j++)
            if (*(syms + i) == *(sup_syms + j))
                break;
        if (j == s->ctrl.num_symbols_supported)
            return BadMatch;
    }

    s->ctrl.num_symbols_displayed = f->num_keysyms;
    for (i = 0; i < f->num_keysyms; i++)
        *(s->ctrl.symbols_displayed + i) = *(syms + i);
    (*s->CtrlProc) (dev, &s->ctrl);
    return Success;
}