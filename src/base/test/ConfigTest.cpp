
#include "../Config.h"
using namespace pallette;
using namespace std;

int main()
{
	Config con("config.ini");

	cout << "name: " << con.read<std::string>("name", "") <<
		"; age: " << con.read<int>("age", 0) << endl;

	con.add<std::string>("name", "liwang");
	con.add<int>("age", 11);
	con.add<double>("score", 13.54);
	con.save();
	cout << "name: " << con.read<std::string>("name", "") <<
		"; age: " << con.read<int>("age", 0) << endl;

	return 0;
}
