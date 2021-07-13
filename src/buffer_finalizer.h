template <typename T>
class BufferFinalizer
{
public:
	void operator()(Napi::Env env, T *data)
	{
		(void)env;
		if (data != nullptr)
		{
			delete data;
		}
	}
};

