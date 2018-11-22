#include <Network/Inc/Network.h>
#include <cassert>

class Foo
{
public:
	Foo()
		: i(0)
		, b(false)
	{}

	void Serialize(Network::StreamWriter& writer)
	{
		writer.Write(i);
		writer.Write(b);
		writer.Write(name);
	}

	void Deserialize(Network::StreamReader& reader)
	{
		reader.Read(i);
		reader.Read(b);
		reader.Read(name);
	}

	int i;
	bool b;
	std::string name;
};

int main()
{
	int outI = 42;
	float outF = 3.14f;
	double outD = 6.28;
	char outC = 'a';
	bool outB = true;
	std::string outStr = "test";

	Network::MemoryStream outStream;
	Network::StreamWriter writer(outStream);
	writer.Write(outI);
	writer.Write(outF);
	writer.Write(outD);
	writer.Write(outC);
	writer.Write(outB);
	writer.Write(outStr);

	int inI = 0;
	float inF = 0.0f;
	double inD = 0.0;
	char inC = '\0';
	bool inB = false;
	std::string inStr = "garbage";

	Network::MemoryStream inStream(outStream.GetData(), outStream.GetSize());
	Network::StreamReader reader(inStream);
	reader.Read(inI);
	reader.Read(inF);
	reader.Read(inD);
	reader.Read(inC);
	reader.Read(inB);
	reader.Read(inStr);

	assert(inI == outI);
	assert(inF == outF);
	assert(inD == outD);
	assert(inC == outC);
	assert(inB == outB);
	assert(inStr == outStr);

	Foo myFoo;
	myFoo.i = 77;
	myFoo.b = true;
	myFoo.name = "jackson";

	Network::MemoryStream fooOut;
	Network::StreamWriter fooWriter(fooOut);
	myFoo.Serialize(fooWriter);

	Network::MemoryStream fooIn(fooOut.GetData(), fooOut.GetSize());
	Network::StreamReader fooReader(fooIn);

	Foo urFoo;
	urFoo.Deserialize(fooReader);

	assert(myFoo.i == urFoo.i);
	assert(myFoo.b == urFoo.b);
	assert(myFoo.name == urFoo.name);

	return 0;
}