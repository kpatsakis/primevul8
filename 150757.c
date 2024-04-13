static inline void invalidate_cursor1(CirrusVGAState *s)
{
    if (s->last_hw_cursor_size) {
        vga_invalidate_scanlines((VGAState *)s,
                                 s->last_hw_cursor_y + s->last_hw_cursor_y_start,
                                 s->last_hw_cursor_y + s->last_hw_cursor_y_end);
    }
}