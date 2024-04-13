void VariationalRefinementImpl::RedBlackBuffer::create(Size s)
{
    /* Allocate enough memory to include borders */
    int w = (int)ceil(s.width / 2.0) + 2;
    red.create(s.height + 2, w);
    black.create(s.height + 2, w);

    if (s.width % 2 == 0)
        red_even_len = red_odd_len = black_even_len = black_odd_len = w - 2;
    else
    {
        red_even_len = black_odd_len = w - 2;
        red_odd_len = black_even_len = w - 3;
    }
}