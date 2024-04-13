get_operator(const node *n)
{
    switch (TYPE(n)) {
        case VBAR:
            return BitOr;
        case CIRCUMFLEX:
            return BitXor;
        case AMPER:
            return BitAnd;
        case LEFTSHIFT:
            return LShift;
        case RIGHTSHIFT:
            return RShift;
        case PLUS:
            return Add;
        case MINUS:
            return Sub;
        case STAR:
            return Mult;
        case SLASH:
            return Div;
        case DOUBLESLASH:
            return FloorDiv;
        case PERCENT:
            return Mod;
        default:
            return (operator_ty)0;
    }
}