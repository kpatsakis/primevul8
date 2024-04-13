parsenumber(struct compiling *c, const char *s)
{
        const char *end;
        long x;
        double dx;
        int old_style_octal;
#ifndef WITHOUT_COMPLEX
        Py_complex complex;
        int imflag;
#endif

        assert(s != NULL);
        errno = 0;
        end = s + strlen(s) - 1;
#ifndef WITHOUT_COMPLEX
        imflag = *end == 'j' || *end == 'J';
#endif
        if (*end == 'l' || *end == 'L') {
                /* Make a copy without the trailing 'L' */
                size_t len = end - s  + 1;
                char *copy = malloc(len);
                PyObject *result;
                if (copy == NULL)
                        return PyErr_NoMemory();
                memcpy(copy, s, len);
                copy[len - 1] = '\0';
                old_style_octal = len > 2 && copy[0] == '0' && copy[1] >= '0' && copy[1] <= '9';
                result = PyLong_FromString(copy, (char **)0, old_style_octal ? 8 : 0);
                free(copy);
                return result;
        }
        x = Ta27OS_strtol((char *)s, (char **)&end, 0);
        if (*end == '\0') {
                if (errno != 0) {
                        old_style_octal = end - s > 1 && s[0] == '0' && s[1] >= '0' && s[1] <= '9';
                        return PyLong_FromString((char *)s, (char **)0, old_style_octal ? 8 : 0);
                }
                return PyLong_FromLong(x);
        }
        /* XXX Huge floats may silently fail */
#ifndef WITHOUT_COMPLEX
        if (imflag) {
                complex.real = 0.;
                complex.imag = PyOS_string_to_double(s, (char **)&end, NULL);
                if (complex.imag == -1.0 && PyErr_Occurred())
                        return NULL;
                return PyComplex_FromCComplex(complex);
        }
        else
#endif
        {
                dx = PyOS_string_to_double(s, NULL, NULL);
                if (dx == -1.0 && PyErr_Occurred())
                        return NULL;
                return PyFloat_FromDouble(dx);
        }
}