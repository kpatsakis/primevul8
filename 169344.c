    bool ImageFactory::checkType(int type, BasicIo& io, bool advance)
    {
        const Registry* r = find(registry, type);
        if (0 != r) {
            return r->isThisType_(io, advance);
        }
        return false;
    } // ImageFactory::checkType