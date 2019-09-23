template <typename T>
class BufferFinalizer
{
public:
	void operator()(Napi::Env env, T *data)
	{
		if (data != nullptr)
		{
			delete data;
		}
	}
};

