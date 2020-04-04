#include "surfaces.hpp"

int main()
{
	struct Screen *screen = IIntuition->LockPubScreen (0);
										    
	StageInterface interface (screen->Width, screen->Height);
	
	interface.populate();
	interface.show();
	interface.loop();
	
	interface.close();
	
	return 0;
}
