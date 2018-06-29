#include <string>
#include <vector>
#include <fstream>
#include <switch.h>
#include <unistd.h>
#include <sys/stat.h>

#include "gfx.h"
#include "data.h"
#include "ui.h"

extern "C"
{
	void userAppInit(void)
	{
		romfsInit();
		hidInitialize();
		nsInitialize();
		setsysInitialize();
		accountInitialize();

		mkdir(".JKSV", 777);
		chdir(".JKSV");
	}

	void userAppExit(void)
	{
		romfsExit();
		hidExit();
		nsExit();
		setsysExit();
		accountExit();
	}
}

extern int mstate;

int main(int argc, const char *argv[])
{
	gfx::init();
	data::loadDataInfo();
	ui::init();
	ui::userMenuInit();

	bool run = true;
	while(appletMainLoop() && run)
	{
		hidScanInput();

		uint64_t down = hidKeysDown(CONTROLLER_P1_AUTO);
		uint64_t held = hidKeysHeld(CONTROLLER_P1_AUTO);

		if((held & KEY_L) && (held & KEY_R) && (held & KEY_ZL) && (held & KEY_ZR))
		{
			if(ui::confirm("You are about to enable system save dumping and remove checks. Are you sure you want to continue?"))
			{
				data::sysSave = true;
				data::forceMountable = false;
				data::loadDataInfo();
				ui::userMenuInit();

				//Just to be sure
				fsdevUnmountDevice("sv");

				mstate = 0;
			}
		}
		else if(down & KEY_PLUS)
			break;

		ui::runApp(down, held);

		gfx::handleBuffs();
	}
	gfx::fini();
}
