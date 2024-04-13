            bool skip( uint64_t sz ) {
                _position += sz;
                return _position < _maxLength;
            }