TileBuffer::TileBuffer (Compressor *comp):
    uncompressedData (0),
    buffer (0),
    dataSize (0),
    compressor (comp),
    format (defaultFormat (compressor)),
    dx (-1),
    dy (-1),
    lx (-1),
    ly (-1),
    hasException (false),
    exception (),
    _sem (1)
{
    // empty
}