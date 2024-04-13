template <class T> T* Statement::Bind(const Napi::CallbackInfo& info, int start, int last) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (last < 0) last = info.Length();
    Napi::Function callback;
    if (last > start && info[last - 1].IsFunction()) {
        callback = info[last - 1].As<Napi::Function>();
        last--;
    }

    T* baton = new T(this, callback);

    if (start < last) {
        if (info[start].IsArray()) {
            Napi::Array array = info[start].As<Napi::Array>();
            int length = array.Length();
            // Note: bind parameters start with 1.
            for (int i = 0, pos = 1; i < length; i++, pos++) {
                baton->parameters.push_back(BindParameter((array).Get(i), pos));
            }
        }
        else if (!info[start].IsObject() || OtherInstanceOf(info[start].As<Object>(), "RegExp") || OtherInstanceOf(info[start].As<Object>(), "Date") || info[start].IsBuffer()) {
            // Parameters directly in array.
            // Note: bind parameters start with 1.
            for (int i = start, pos = 1; i < last; i++, pos++) {
                baton->parameters.push_back(BindParameter(info[i], pos));
            }
        }
        else if (info[start].IsObject()) {
            Napi::Object object = info[start].As<Napi::Object>();
            Napi::Array array = object.GetPropertyNames();
            int length = array.Length();
            for (int i = 0; i < length; i++) {
                Napi::Value name = (array).Get(i);
                Napi::Number num = name.ToNumber();

                if (num.Int32Value() == num.DoubleValue()) {
                    baton->parameters.push_back(
                        BindParameter((object).Get(name), num.Int32Value()));
                }
                else {
                    baton->parameters.push_back(BindParameter((object).Get(name),
                        name.As<Napi::String>().Utf8Value().c_str()));
                }
            }
        }
        else {
            return NULL;
        }
    }

    return baton;
}