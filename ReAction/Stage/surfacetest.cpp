#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "surfaces.hpp"

void do_animate(Stage &stage, struct Window *w, struct RastPort *rp)
{
	while (stage.animate())
	{
	    IGraphics->RectFillColor (rp, 0, 0, stage.totalWidth(), stage.totalHeight(), 0xff000000);     
	    
		stage.render (rp);
				
		IGraphics->BltBitMapRastPort (rp->BitMap, 0, 0, w->RPort, 0, 0, stage.totalWidth(), stage.totalHeight(), 0xc0);
		IGraphics->WaitTOF();
	}
}

int main()
{
    struct Screen *screen = IIntuition->LockPubScreen (NULL);

    uint32 screen_width = screen->Width;
    uint32 screen_height = screen->Height;

    struct Window *surface_window = IIntuition->OpenWindowTags(NULL,
        WA_Width, screen->Width,
        WA_Height, screen->Height,
        WA_ReportMouse, TRUE,
        WA_Activate, TRUE,
        WA_IDCMP, IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_VANILLAKEY,
        TAG_DONE);

    if (surface_window)
    {
		struct BitMap *render_bitmap = IGraphics->AllocBitMapTags (screen->Width, screen->Height, 32,
															BMATags_PixelFormat, PIXF_A8R8G8B8,
															TAG_DONE);
		if (render_bitmap)
		{
			struct RastPort rp;
			IGraphics->InitRastPort (&rp);
			rp.BitMap = render_bitmap;

		    IGraphics->RectFillColor (&rp, 0, 0, screen_width, screen_height, 0xff000000);     
				
			Stage stage (screen_width, screen_height);
	
			stage.createRow();
			stage.createEmptySurface(0);
			stage.createEmptySurface(0);
			stage.createRow();
			stage.createEmptySurface(1);
			stage.createEmptySurface(1);
			stage.createRow();
			stage.createEmptySurface(2);

			stage.calculateTargetDimensions(screen_width, screen_height);
			
			do_animate (stage, surface_window, &rp);
			
			stage.createEmptySurface (2);
			stage.calculateTargetDimensions(screen_width, screen_height);

			do_animate (stage, surface_window, &rp);
			
			Surface *s = stage.take(0, 1);
			stage.calculateTargetDimensions(screen_width, screen_height);
			
			do_animate (stage, surface_window, &rp);

			stage.insertSurface(s, 1, 0);
			stage.calculateTargetDimensions(screen_width, screen_height);

			do_animate (stage, surface_window, &rp);

			s = stage.take (2, 0);
			stage.calculateTargetDimensions(screen_width, screen_height);
			
			do_animate (stage, surface_window, &rp);

			s = stage.take (2, 0);
			stage.calculateTargetDimensions(screen_width, screen_height);
			
			do_animate (stage, surface_window, &rp);
			
			s = stage.take (0, 0);
			stage.calculateTargetDimensions(screen_width, screen_height);
			
			do_animate (stage, surface_window, &rp);
		}
		
		IGraphics->FreeBitMap (render_bitmap);
	}
	
	IIntuition->CloseWindow (surface_window);
	return 0;
}
