void OutOfMem(size_t size)
{
    FatalError("Out of memory on allocating %d bytes.", size);
}