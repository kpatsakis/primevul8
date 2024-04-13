int get_format_position(unsigned int oformat)
{
        int i;

        for (i = 0; i < NR_FMT; i++) {
                if (fmt[i]->id == oformat)
                        break;
        }

        if (i == NR_FMT)
		/* Should never happen */
                return 0;

        return i;
}