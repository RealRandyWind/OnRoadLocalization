#include "Main.hpp"

int main()
{
	Main* oApplication = Main::GetInstance();
	int iEntryReturn = oApplication->Entry();
	return iEntryReturn;
}
