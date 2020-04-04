
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/bsdsocket.h>

#include <intuition/gadgetclass.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "surfaces.h"
#include "preferences.h"
#include "gui.h"

#include "console.h"
#include "sourcelist.h"
#include "sourcewindow.h"
#include "variables.h"
#include "stacktrace.h"

struct Window *surfacewindow = NULL;

struct BitMap *render_bitmap = NULL;
int screen_width, screen_height;

void move_drag_window();

void draw_green_box_around_thumbnail_area (struct RastPort *rp, struct grid *g)
{
	//box
	IGraphics->RectFillColor (rp, (int)0, (int)0, (int)g->width, 4, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)0, (int)0, 4, (int)g->space_above+1, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)g->width-4, (int)0, (int)g->width, (int)g->space_above+1, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)0, (int)g->space_above-4, (int)g->width, (int)g->space_above+1, 0xff00ee22);
}

void draw_green_box_around_surface (struct RastPort *rp, struct surface *s)
{
	//box
	IGraphics->RectFillColor (rp, (int)s->x-1, (int)s->y-1, (int)s->x+(int)s->width+1, (int)s->y + 4, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)s->x-1, (int)s->y-1, (int)s->x+4, (int)s->y+(int)s->height+1, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)s->x+(int)s->width-4, (int)s->y-1, (int)s->x+(int)s->width+1, (int)s->y+(int)s->height+1, 0xff00ee22);
	IGraphics->RectFillColor (rp, (int)s->x-1, (int)s->y+(int)s->height-4, (int)s->x+(int)s->width+1, (int)s->y+(int)s->height+1, 0xff00ee22);

	//text
	int length = IGraphics->TextLength (rp, s->name, strlen (s->name));
	IGraphics->SetRPAttrs (rp, RPTAG_APenColor, 0xff00ee22, TAG_DONE);
	IGraphics->Move (rp, (int)s->x + (int)s->width / 2 - length / 2, (int)s->y + (int)s->height / 2 - 24 / 2);
	IGraphics->Text (rp, s->name, strlen(s->name));
}

void paint (struct grid *g)
{
	// check if window is moving
    struct IntuiMessage *msg = (struct IntuiMessage *)IExec->GetMsg (surfacewindow->UserPort);
    while (msg)
    {
        if (msg->Class == IDCMP_MOUSEMOVE)
            move_drag_window();
        IExec->ReplyMsg ((struct Message *)msg);
        msg = (struct IntuiMessage *) IExec->GetMsg (surfacewindow->UserPort);
    }

	// clear screen
    struct RastPort rastport;
    IGraphics->InitRastPort (&rastport);
    rastport.BitMap = render_bitmap;

    IGraphics->RectFillColor (&rastport, 0, 0, g->width, g->height, 0xff000000);     
				
	// do the actual graphics
    for (int i = 0; i < g->rows_available; i++)
    {
        struct surface *s = g->rows[i];
        for (int j = 0; j < g->surfaces_per_row[i]; j++)
		{	
            float scalex = s->width / s->original_width;
            float scaley = s->height / s->original_height;

			if (!s->empty_space)
			{
#if 1
				/*uint32 result =*/
				IGraphics->CompositeTags(COMPOSITE_Src_Over_Dest, s->bitmap, render_bitmap,
							COMPTAG_SrcX, 0,
							COMPTAG_SrcY, 0,
							COMPTAG_SrcWidth, (int)s->original_width,
							COMPTAG_SrcHeight, (int)s->original_height,
							COMPTAG_OffsetX, (int)s->x,
							COMPTAG_OffsetY, (int)s->y,
							COMPTAG_DestX, (int)s->x,
							COMPTAG_DestY, (int)s->y,
							COMPTAG_DestWidth, (int)s->width,
							COMPTAG_DestHeight, (int)s->height,
							COMPTAG_ScaleX, COMP_FLOAT_TO_FIX(scalex),
							COMPTAG_ScaleY, COMP_FLOAT_TO_FIX(scaley),
							TAG_DONE);
#endif
				if (s->infocus)
					draw_green_box_around_surface (&rastport, s);
			}
			else
			{
                IGraphics->RectFillColor (&rastport, s->x, s->y, s->x + s->width, s->y + s->height, 0xff666666);
			}				
			
            s = s->next;
        }
    }
	
	//paint the thumbnails at the top of the screen
	
	float x1 = g->width / 2.0f - 200.0f * g->thumbs_available / 2.0f;
	float width = 150.0f;
	float height = g->space_above - 10.0f;
	float offset = 200.0f;
	
	for (int i = 0; i < g->thumbs_available; i++)
	{
		struct surface *s = g->thumbnails[i];
		
		float scalex = width / s->original_width;
		float scaley = height / s->original_height;

		s->x = x1;
		s->y = 5.0f;
		s->width = width;
		s->height = height;

		/*uint32 result =*/
		IGraphics->CompositeTags(COMPOSITE_Src_Over_Dest, s->bitmap, render_bitmap,
							COMPTAG_SrcX, 0,
							COMPTAG_SrcY, 0,
							COMPTAG_SrcWidth, (int)s->original_width,
							COMPTAG_SrcHeight, (int)s->original_height,
							COMPTAG_OffsetX, (int)x1,
							COMPTAG_OffsetY, (int)5.0f,
							COMPTAG_DestX, (int)x1,
							COMPTAG_DestY, (int)5.0f,
							COMPTAG_DestWidth, (int)width,
							COMPTAG_DestHeight, (int)height,
							COMPTAG_ScaleX, COMP_FLOAT_TO_FIX(scalex),
							COMPTAG_ScaleY, COMP_FLOAT_TO_FIX(scaley),
							TAG_DONE);
		
		x1 += offset;
	}
	
	if (g->drag_to_thumbnail_area)
		draw_green_box_around_thumbnail_area (&rastport, g);
		
    IGraphics->BltBitMapRastPort (render_bitmap, 0, 0, surfacewindow->RPort, 0, 0, screen_width, screen_height, 0xc0);
    IGraphics->WaitTOF();
}

//
//
// Drag window
//
//

static int drag_width, drag_height;

struct Window *drag_window = NULL;
extern struct Screen *public_screen;

void open_drag_window (int width, int height)
{
    struct Screen *screen = IIntuition->LockPubScreen (NULL);

    if (screen)
    {
        struct Window *window = IIntuition->OpenWindowTags (NULL,
            WA_Left, screen->MouseX - width/2,
            WA_Top, screen->MouseY - height/2,
            WA_Width, width,
            WA_Height, height,
            WA_ToolBox, TRUE,
            WA_Borderless, TRUE,
            WA_PubScreen, public_screen,
            WA_Opaqueness, 0x99,
            TAG_DONE);

        IGraphics->RectFillColor (window->RPort, 1, 1, width-2, height-2, 0xffeeffee);

        drag_width = width;
        drag_height = height;

        IIntuition->UnlockPubScreen (NULL, screen);

        drag_window = window;
    }
}

void close_drag_window ()
{
    if (drag_window)
        IIntuition->CloseWindow (drag_window);
    drag_window = NULL;
}

void move_drag_window ()
{
    struct Screen *screen = IIntuition->LockPubScreen (NULL);

    if (screen)
    {
        IIntuition->SetWindowAttrs (drag_window,
            WA_Left, screen->MouseX - drag_width/2,
            WA_Top, screen->MouseY - drag_height/2,
            TAG_DONE);

        IIntuition->UnlockPubScreen (NULL, screen);
    }
}

//
//
// Show Surfaces
//
//

void remove_space (struct grid *g, int row, int index)
{
    struct surface *s1 = g->rows[row];
    struct surface *ps = 0;
    for (int i = 0; i < index; i++)
    {
        ps = s1;
        s1 = s1->next;
    }

    if (s1)
    {
        if (ps)
            ps->next = s1->next;
        else
            g->rows[row] = s1->next;
    }
    g->surfaces_per_row[row]--;
    
    IExec->FreeVec (s1);
}

void show_surfaces (struct grid *g)
{
    struct Screen *screen = IIntuition->LockPubScreen (NULL);

    screen_width = screen->Width;
    screen_height = screen->Height;

    surfacewindow = IIntuition->OpenWindowTags(NULL,
        WA_Width, screen->Width,
        WA_Height, screen->Height,
        WA_ReportMouse, TRUE,
        WA_PubScreen, public_screen,
        WA_Activate, TRUE,
        WA_IDCMP, IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_VANILLAKEY,
        TAG_DONE);

	render_bitmap = IGraphics->AllocBitMapTags (screen->Width, screen->Height, 32,
                                                            BMATags_PixelFormat, PIXF_A8R8G8B8,
                                                            TAG_DONE);

    if (surfacewindow)
    {
        read_surface_states (g);

		paint (g);

		BOOL done = FALSE;

		static struct surface *empty_space_surface = 0;

        BOOL drag_is_in_progress = FALSE;

		struct surface *drag_surface = NULL;
		
		struct timeval t = { 0L, 1000L };
		BOOL timerwait = FALSE;
		
        while (!done)
        {
			timerwait = animate (g, paint);
        	
			t.tv_usec = 100;
			ULONG sig = 1 << surfacewindow->UserPort->mp_SigBit;
			
			ISocket->WaitSelect (0, NULL, NULL, NULL, (timerwait ? &t : NULL), &sig);

            struct IntuiMessage *msg = (struct IntuiMessage *)IExec->GetMsg (surfacewindow->UserPort);

            while (msg)
            {
                int mousex = msg->MouseX;
                int mousey = msg->MouseY;
                uint32 type = msg->Class;

                IExec->ReplyMsg ((struct Message *)msg);

                switch (type)
                {
                    case IDCMP_MOUSEBUTTONS:

                    if (msg->Code == SELECTDOWN)
                    {
                        int row, index;
                        struct surface *s = surface_at_point (g, mousex, mousey, &row, &index);

						if (s)
						{
							s->original_row = row;
							s->original_index = index;

	                        if (drag_is_in_progress)
	                        {
	                        	//There is a mixup.
	                        	//Get rid of drag mode as quickly as possible (this might happen if events are swallowed or mixed up)
	                        	
								close_drag_window();

	                        	insert_grid_element_into_row (g, drag_surface, drag_surface->original_row, drag_surface->original_index);
								
								drag_is_in_progress = FALSE;
								
								if (empty_space_surface)
								{
									free_particular_surface (g, empty_space_surface);
									empty_space_surface = 0;
								}
	                        }
	                        else
    	                    {
                                open_drag_window (s->width, s->height);

                                struct surface *s1 = free_surface_from_row (g, row, index);
								
								if (s1->empty_space)
								{
									free_particular_surface (g, s1);
								}
								else
								{
	                                if (g->rows[row] == NULL)
										remove_row_from_grid (g, row);
									
									drag_is_in_progress = TRUE;
									drag_surface = s1;
								}
                            }
                        }
                        else
                        {
                        	struct surface *th = thumbnail_at_point (g, mousex, mousey);
                        	
                        	if (th)
                        	{
                                open_drag_window (th->width, th->height);

                                remove_thumbnail_from_grid (g, th);
								
								drag_is_in_progress = TRUE;
								drag_surface = th;
							}
						}
                    }
                  
                    if (msg->Code == SELECTUP)
                    {
						if (!drag_is_in_progress)
							break;
                    		
                        close_drag_window();

                        drag_is_in_progress = FALSE;
						g->drag_to_thumbnail_area = FALSE;
						
						int row, index;
                        space_type t = identify_area (g, mousex, mousey, &row, &index);                    

						//printf("DROP: space_type: %d empty_space_surface: 0x%x\n", (int)t, empty_space_surface);

						switch (t)
						{
							case SPACE_SURFACE:
							case SPACE_UNKNOWN:
							case SPACE_EDGE:							
							
								// throw it back where it came from
#if 1
								if (drag_surface->original_row == g->rows_available)
								{
									//the row might have been removed
									
									free_particular_surface (g, empty_space_surface);
									empty_space_surface = insert_empty_row_into_grid (g, drag_surface->original_row);
								}
#endif
								insert_grid_element_into_row (g, drag_surface, drag_surface->original_row, drag_surface->original_index);
								
								break;
								
							case SPACE_EMPTY_SPACE:
							
								insert_grid_element_into_row (g, drag_surface, row, index);
								
								break;
								
							case SPACE_SPACE_ABOVE:
							
								g->thumbnails[g->thumbs_available] = drag_surface;
								g->thumbs_available++;
								
								break;
                        }
                        
                        if (empty_space_surface)
						{
							free_particular_surface (g, empty_space_surface);
							empty_space_surface = 0;
						}
						
                    }
                
                    break;

                    case IDCMP_MOUSEMOVE:

                    if (drag_is_in_progress)
                    {
                        if (drag_window)
                        {
                            struct Screen *screen = IIntuition->LockPubScreen (NULL);

                            if (screen)
                            {
                                move_drag_window (drag_window);
                            }
							
							g->drag_to_thumbnail_area = FALSE;
							
							int row, index;
							space_type t = identify_area (g, mousex, mousey, &row, &index);

							switch (t)
							{
								case SPACE_EMPTY_SPACE:
							
									break;
							
								case SPACE_SURFACE:
	
									if (empty_space_surface)
									{
										free_particular_surface (g, empty_space_surface);
									}
									break;
								
								case SPACE_SPACE_ABOVE:
								
									if (empty_space_surface)
									{
										free_particular_surface (g, empty_space_surface);
									}
									
									g->drag_to_thumbnail_area = TRUE;
									
									break;
								
								case SPACE_EDGE:
							
									if (empty_space_surface)
									{
										free_particular_surface (g, empty_space_surface);
										empty_space_surface = 0;
									}
								
									if (index == -1)
									{
										empty_space_surface = insert_empty_row_into_grid (g, row);
									}
									else
									{
										empty_space_surface = insert_empty_space_into_row (g, row, index);
									}
									break;
							
								default:
							
									break;
							}                  
                        }
                    }
                    else
                    {
                    	static struct surface *lasts = 0;
                        
                        struct surface *s = surface_at_point (g, msg->MouseX, msg->MouseY, NULL, NULL);
                        
                        if(s)
                            s->infocus = TRUE;

						if(lasts && lasts != s)
							lasts->infocus = FALSE;

						lasts = s;
                    }

                    break;

                    case IDCMP_VANILLAKEY:

                        done = TRUE;
                        break;

                    default:
                        break;
                }

                msg = (struct IntuiMessage *)IExec->GetMsg (surfacewindow->UserPort);
            }
        }

		//write_result_to_prefs(g);
				
        IGraphics->FreeBitMap (render_bitmap);
        render_bitmap = NULL;

        IIntuition->CloseWindow (surfacewindow);
    }

    return;
}

void write_result_to_prefs (struct grid *g)
{
	int i;
	for (i = 0; i < g->rows_available; i++)
	{
		struct surface *s = g->rows[i];
		
		int j;
		for (j = 0; j < g->surfaces_per_row[i]; j++)
		{
			db101_prefs.prefs_surfaces[i][j] = s->element;
			s = s->next;
		}
		db101_prefs.prefs_surfaces[i][j] = ELEMENT_NOELEMENT;
	}
	db101_prefs.prefs_surfaces[i][0] = ELEMENT_NOELEMENT;
}

// // // //
//
// Grid
//
// // // // // // // // //

void create_grid (struct grid *g)
{
    int i;
	for (i = 0; i < 5 && db101_prefs.prefs_surfaces[i][0] != ELEMENT_NOELEMENT; i++)
    {
        struct surface *s = IExec->AllocVecTags (sizeof (struct surface), TAG_DONE);
        g->rows[i] = s;

        int j;
        for (j = 0; j < 5 && db101_prefs.prefs_surfaces[i][j] != ELEMENT_NOELEMENT; j++)
        {
            switch (db101_prefs.prefs_surfaces[i][j])
            {
                case ELEMENT_STACKTRACE:
                    stacktrace_prepare_surface (s);
                    break;

                case ELEMENT_VARIABLES:
                    variables_prepare_surface (s);
                    break;

                case ELEMENT_SOURCECODE:
                    sourcecode_prepare_surface (s);
                    break;

                case ELEMENT_SOURCELIST:
                    sourcelist_prepare_surface (s);
                    break;

                case ELEMENT_CONSOLE:
                    console_prepare_surface (s);
                    break;

                default:
                    printf("Error!\n");
                    break;
            }
            if (db101_prefs.prefs_surfaces[i][j+1] != ELEMENT_NOELEMENT)
            {
            	struct surface *ns = IExec->AllocVecTags (sizeof (struct surface), TAG_DONE);
            	s->next = ns;
                s = ns;
            }
        	else
            	s->next = NULL;
        }
        g->surfaces_per_row[i] = j;
    }
    g->rows_available = i;
    
    g->thumbs_available = 0;
    g->drag_to_thumbnail_area = FALSE;
}

struct grid *alloc_grid ()
{
	struct Screen *screen = IIntuition->LockPubScreen (NULL);

	struct grid *g = IExec->AllocVecTags (sizeof (struct grid), TAG_DONE);

    if (g)
    {
    	g->width = screen->Width;
    	g->height = screen->Height;
		g->padding_outer = 2.0f;
		g->padding_between_surfaces = 10.0f;
		g->space_above = 55.0f;
		
	    create_grid (g);
    }

    IIntuition->UnlockPubScreen (NULL, screen);

    return g;
}

void destroy_grid (struct grid *g)
{
    for (int i = 0; i < g->rows_available; i++)
    {
        struct surface *s = g->rows[i];

        for (int j = 0; j < g->surfaces_per_row[i]; j++)
        {
            struct surface *ns = s->next;
            IExec->FreeVec (s);
            s = ns;
        }
    }
    IExec->FreeVec (g);
}

void read_surface_states (struct grid *g)
{
	struct RastPort rastport;
	IGraphics->InitRastPort (&rastport);
	
	for (int i = 0; i < g->rows_available; i++)
	{
		struct surface *s = g->rows[i];

		for (int j = 0; j < g->surfaces_per_row[i]; j++)
		{
			int x, y, width, height;
             
			IIntuition->GetAttrs (s->object,
                                    GA_Left, &x,
                                    GA_Top, &y,
                                    GA_Width, &width,
                                    GA_Height, &height,
                                    TAG_DONE);
                                    
			s->x = (float)x;
			s->y = (float)y;
			s->width = (float)width;
			s->height = (float)height;

			s->original_x = (float)x;
			s->original_y = (float)y;
			s->original_width = (float)width;
			s->original_height = (float)height;
						
			s->target_x = (float)x;
			s->target_y = (float)y;
			s->target_width = (float)width;
			s->target_height = (float)height;
			/*
			s->render_x = (float)x;
			s->render_y = (float)y;
			s->render_width = (float)width;
			s->render_height = (float)height;
			*/
			s->infocus = FALSE;
			s->empty_space = FALSE;
			
			s->bitmap = IGraphics->AllocBitMapTags (s->original_width, s->original_height, 32,
                                                            BMATags_PixelFormat, PIXF_A8R8G8B8,
                                                            BMATags_Friend, mainwin->RPort->BitMap,
                                                            TAG_DONE);

			uint8 *buffer = IExec->AllocVecTags(s->original_width * s->original_height * 4, TAG_DONE);

			IGraphics->ReadPixelArray(mainwin->RPort,
            							s->original_x, s->original_y,
										buffer, 0, 0,
										4*s->original_width, PIXF_A8R8G8B8,
										s->original_width, s->original_height);

			rastport.BitMap = s->bitmap;

			IGraphics->WritePixelArray(buffer,
										0, 0,
										4*s->original_width, PIXF_A8R8G8B8,
										&rastport, 0, 0,
										s->original_width, s->original_height);

			IExec->FreeVec (buffer);
			
            s = s->next;
        }
    }
}


// // // // /
// / API // //
// // // // / / // / / // /   /

struct surface *surface_at_point (struct grid *g, int x, int y, int *row, int *index)
{
	int i;
    for (i = 0; i < g->rows_available; i++)
    {
        struct surface *s = g->rows[i];
        
        int j;
		for (j = 0; j < g->surfaces_per_row[i]; j++)
		{
			if (x >= (int)s->x &&
				y >= (int)s->y &&
				x < (int)(s->x + s->width) &&
				y < (int)(s->y + s->height)
				)
			{
				if (row)
					*row = i;
				if (index)
					*index = j;
				return s;
			}
			
			s = s->next;
		}
	}
	
	return NULL;
}

struct surface *thumbnail_at_point (struct grid *g, int x, int y)
{
	int i;
	for (i = 0; i < g->thumbs_available; i++)
	{
		struct surface *s = g->thumbnails[i];
		
			if (x >= (int)s->x &&
				y >= (int)s->y &&
				x < (int)(s->x + s->width) &&
				y < (int)(s->y + s->height)
				)
				return s;
	}
	return 0;
}

void remove_thumbnail_from_grid (struct grid *g, struct surface *th)
{
	int i;
	for (i = 0; i < g->thumbs_available; i++)
	{
		if (th == g->thumbnails[i])
		{
			int j;
			for (j = i; j < g->thumbs_available; j++)
			{
				g->thumbnails[j] = g->thumbnails[j+1];
			}
			g->thumbs_available--;
		}
	}
}

BOOL SPACE_next_to_space (struct grid *g, int row, int index, float _x)
{
	int x = (int)_x;
	
	if (index == -1)
	{
		//check above
		
		if (row)
		{
			if (g->surfaces_per_row[row-1] == 0 || g->rows[row-1] == NULL)
				return TRUE;
				
			struct surface *s = g->rows[row-1];
			int j;
			for (j = 0; j < g->surfaces_per_row[row-1]; j++)
			{
				if (s->empty_space && x > s->x && x < s->x + s->width)
					return TRUE;
				s = s->next;
			}
		}		
		
		//check below
		if (row < g->rows_available)
		{
			struct surface *s = g->rows[row];
			int j;
			for (j = 0; j < g->surfaces_per_row[row]; j++)
			{
				if (s->empty_space && x > s->x && x < s->x + s->width)
					return TRUE;
				s = s->next;
			}
		}
		
		return FALSE;	
	}
	else
	{
		if (g->surfaces_per_row[row] == 0)
			return TRUE;
			
		if (index == 0)
		{
			if (g->rows[row]->empty_space)
				return TRUE;
		}
		else if (index == g->surfaces_per_row[row] - 1)
		{
			struct surface *s = g->rows[row];
			
			int j;
			for (j = 0; j < g->surfaces_per_row[row] - 1; j++)
			{
				s = s->next;
			}
			
			if (s && s->empty_space)
				return TRUE;
		}
		else
		{
			struct surface *s = g->rows[row];
			struct surface *sprev = s;
			
			int j;
			for (j = 0; j < index; j++)
			{
				sprev = s;
				s = s->next;
			}
			
			if ((s && s->empty_space) || (sprev && sprev->empty_space))
				return TRUE;
		}
	}
	return FALSE;
}
			
space_type identify_area (struct grid *g, int x, int y, int *row, int *index)
{
	int _row, _index;
    struct surface *s = surface_at_point (g, x, y, &_row, &_index);

	if (!s)
	{
		if (y < g->space_above)
			return SPACE_SPACE_ABOVE;
			
		int i;
		for (i = 0; i < g->rows_available; i++)
		{
			s = g->rows[i];
			
			float surface_area_height = g->height - g->space_above - g->padding_outer * 2.0f;
			float realestate_height = surface_area_height - (float)(g->rows_available - 1) * g->padding_between_surfaces;
			
			float y1 = g->space_above + g->padding_between_surfaces * (float)i + (float)i * realestate_height / (float)(g->rows_available);

			if ((float)y > y1 - g->padding_between_surfaces &&
				(float)y < y1)
			{
				*row = i;
				*index = -1;
				
				//if (SPACE_next_to_space (g, i, -1, x))
				//	return SPACE_EMPTY_SPACE;
									
				return SPACE_EDGE;
			}

			if ((float)y > y1 && (float)y < y1 + realestate_height / (float)(g->rows_available))
			{
				int j;
				for (j = 0; j < g->surfaces_per_row[i]; j++)
				{
					float surface_area_width = g->width - g->padding_outer * 2.0f;
					float realestate_width = surface_area_width - (float)(g->surfaces_per_row[i] - 1) * g->padding_between_surfaces;
				
					float x1 = s->x; //g->padding_outer + g->padding_between_surfaces * (float)j + (float)j * realestate_width / (float)(g->surfaces_per_row[i]);

					if (!j && x <= g->padding_outer ||
						( (float)x >= x1 - (j ? g->padding_between_surfaces : g->padding_outer) &&
						(float)x <= x1 )
						)
					{
						
						*row = i;
						*index = j;
					
						//if (SPACE_next_to_space (g, i, j, x))
						//	return SPACE_EMPTY_SPACE;
							
						return SPACE_EDGE;
					}
				
					if (s->next)
						s = s->next;
				}
				
				if ((float)x >= g->width - g->padding_outer)
				{
					*row = i;
					*index = j;				

					if (SPACE_next_to_space (g, i, j, x))
						return SPACE_EMPTY_SPACE;
					
					return SPACE_EDGE;
				}
			}
		}
		
		if ((float)y >= g->height - g->padding_outer)
		{
			*row = i;
			*index = -1;
			
			if (SPACE_next_to_space (g, i, -1, x))
				return SPACE_EMPTY_SPACE;
			
			return SPACE_EDGE;
		}
			
		//dunno what to do
		return SPACE_UNKNOWN;
	}

	if (row)
		*row = _row;
	if (index)
		*index = _index;

	if (s && s->empty_space)
		return SPACE_EMPTY_SPACE;

    return SPACE_SURFACE;
}

void calculate_row_dimensions (struct grid *g, int row)
{

	///
	
	float realestate_width = g->width - g->padding_outer * 2.0f - g->padding_between_surfaces * (float)(g->surfaces_per_row[row] - 1);
	float average_target_width = realestate_width / (float)(g->surfaces_per_row[row]);

	//	
	// first pass - determine the minimum
	//
		
    struct surface *s = g->rows[row];
    
	//counter for expandable surfaces (compared to max_width and leftovers from pass 1)
	int n = 0;
	float max_expansion_all = 0.0f;
	
  	int i;   
	for (i = 0; i < g->surfaces_per_row[row]; i++)
	{
		s->target_width = MIN(average_target_width, s->max_width);
		max_expansion_all += MAX(0.0f, s->max_width - s->target_width);
		
		if (s->max_width > s->target_width)
			n++;
			
		realestate_width -= s->target_width;
		
		s = s->next;
	}
	
	//
	// second pass - expand according to leftover realestate
	//

	if (realestate_width > 0)
	{
		s = g->rows[row];
		for (i = 0; i < g->surfaces_per_row[row]; i++)
		{
			if (n && realestate_width && s->target_width < s->max_width)
			{
				float max_expansion_s = MIN(max_expansion_all, MIN(s->max_width - s->target_width, realestate_width));
				float expansion = max_expansion_s; //MIN(realestate_width / n, 
				max_expansion_all -= expansion;
				s->target_width += expansion;

				n--;
				realestate_width -= expansion;
			}
			s = s->next;
		}
	}

	//		
	// third pass - calculate the target positions
	//
	
	float x = 0.0f + g->padding_outer;
	float y1 = g->space_above + g->padding_outer + (float)row * (g->height - g->space_above - g->padding_outer * 2.0f) / g->rows_available + (row ? g->padding_between_surfaces / 2 : 0.0f);

	float h1 = (g->height - g->padding_outer * 2.0f - g->space_above - (float)(g->rows_available - 1) * g->padding_between_surfaces) / g->rows_available;

	s = g->rows[row];

	for (i = 0; i < g->surfaces_per_row[row]; i++)
	{
		s->target_x = x;
		s->target_y = y1;
		s->target_height = h1;

		x += s->target_width + g->padding_between_surfaces;
						
		s = s->next;
	}
}

struct surface *insert_empty_space_into_row (struct grid *g, int row, int position)
{
	struct surface *s = IExec->AllocVecTags (sizeof (struct surface), TAG_DONE);
	s->empty_space = TRUE;
	s->next = 0;
	s->max_width = g->width;
	
	insert_grid_element_into_row (g, s, row, position);
	return s;
}

void insert_grid_element_into_row (struct grid *g, struct surface *ns, int row, int index)
{
	//
	// The actual insertion
	//
	
	if (index == 0)
	{
		ns->next = g->rows[row];
		g->rows[row] = ns;
	}
	else
	{
		struct surface *prevs = g->rows[row];
		
		int i;
		for (i = 0; i < index-1; i++)
		{
			prevs = prevs->next;
		}
		
		ns->next = prevs->next;
		prevs->next = ns;
	}
	
	g->surfaces_per_row[row]++;

	// 
	// Calculate dimensions
	//

	calculate_row_dimensions (g, row);

	ns->width = 0.0f;
	ns->height = 0.0f;
	ns->x = ns->target_x + ns->target_width / 2;
	ns->y = ns->target_y + ns->target_height / 2;
}

void free_particular_surface (struct grid *g, struct surface *s)
{
	int i;
	for (i = 0; i < g->rows_available; i++)
	{
		struct surface *s1 = g->rows[i];
		int j;
		for (j = 0; j < g->surfaces_per_row[i]; j++)
		{
			if (s1 == s)
			{
				if (g->surfaces_per_row[i] == 1)
				{
					remove_row_from_grid (g, i);
				}
				else
				{
					struct surface *s = free_surface_from_row (g, i, j);
					IExec->FreeVec(s);
				}
				
				return;
			}
			s1 = s1->next;
		}
	}
}

struct surface *free_surface_from_row (struct grid *g, int row, int position)
{
	//
	// firstly: remove surface to be freed
	//

    struct surface *s1 = g->rows[row];
    struct surface *ps = 0;
    for (int i = 0; i < position; i++)
    {
        ps = s1;
        s1 = s1->next;
    }

    if (s1)
    {
        if (ps)
            ps->next = s1->next;
        else
            g->rows[row] = s1->next;
    }
    g->surfaces_per_row[row]--;

	//
	// Calculate dimensions
	//

	calculate_row_dimensions (g, row);
	
	//////
	
	return s1;
}

struct surface *insert_empty_row_into_grid (struct grid *g, int insert_row)
{
	if (insert_row > g->rows_available)
		printf("ERROR: row insertion overflow\n");

	struct surface *s1 = 0;
	int i;
    for (i = g->rows_available; i > insert_row; i--)
    {
        g->rows[i] = g->rows[i-1];
        g->surfaces_per_row[i] = g->surfaces_per_row[i-1];
    }
	g->rows_available++;
	g->surfaces_per_row[insert_row] = 0;
	
    for (i = 0; i < g->rows_available; i++)
    {
    	struct surface *s = g->rows[i];    	

   		float realestate_height = g->height - g->space_above - g->padding_outer * 2 - (float)(g->rows_available - 1)  * g->padding_between_surfaces; 
   	   	float y1 = g->space_above + g->padding_outer + (float)i * realestate_height / (float)(g->rows_available) + g->padding_between_surfaces * ((float)i + 0.5f);
		float h1 = realestate_height / (float)(g->rows_available);

    	if (i == insert_row)
    	{
		    s1 = IExec->AllocVecTags (sizeof (struct surface), TAG_DONE);
		    if (s1)
		    {
				s1->empty_space = TRUE;
			
				s1->target_x = g->padding_outer;
			    s1->target_y = y1;
			    s1->target_height = h1;
			    s1->target_width = g->width - 2.0f * g->padding_outer;
				s1->x = s1->target_x;
				s1->y = (i ? (s ? s->y+s->height+g->padding_between_surfaces : 0.0f) : g->space_above);
				s1->width = g->width;
				s1->height = 1;
					
				g->rows[insert_row] = s1;
				g->surfaces_per_row[insert_row] = 1;
				s1->next = NULL;
			}
		}
		else
		{
			int j;
			for (j = 0; j < g->surfaces_per_row[i]; j++)
			{
			    s->target_y = y1;
			    s->target_height = h1;

				s = s->next;
	    	}
    	}		
	}

	return s1;
}

void remove_row_from_grid (struct grid *g, int row)
{
    struct surface *s = g->rows[row];
    while(s)
    {
        struct surface *ns = s->next;
        IExec->FreeVec (s);
        s = ns;
    }

    for (int i = row; i < g->rows_available-1; i++)
    {
        g->rows[i] = g->rows[i+1];
        g->surfaces_per_row[i] = g->surfaces_per_row[i+1];
    }

    g->rows_available--;
    
	float realestate_height = g->height - g->space_above - g->padding_outer * 2 - (float)(g->rows_available)  * g->padding_between_surfaces; 
	float h1 = realestate_height / (float)(g->rows_available);
	
    int i;
    for (i = 0; i < g->rows_available; i++)
    {
    	struct surface *s = g->rows[i];    	

   	   	float y1 = g->space_above + g->padding_outer + (float)i * realestate_height / (float)(g->rows_available) + g->padding_between_surfaces * ((float)i + 0.5f);

		int j;
		for (j = 0; j < g->surfaces_per_row[i]; j++)
		{
		    s->target_y = y1;
		    s->target_height = h1;

			s = s->next;
    	}		
	}
}

void stretch_to_edges (struct grid *g, struct surface *s, int row, int index)
{
	struct surface *sprev = NULL;
	if(index != 0)
	{
		sprev = g->rows[row];
		
		int j;
		for (j = 0; j < index-1; j++)
			sprev = sprev->next;
	}
	
	s->x = (sprev ? sprev->x + sprev->width : 0);
	s->y = (row ? g->rows[row-1]->y + g->rows[row-1]->height : g->space_above);
	s->width = (s->next ? s->next->x : g->width) - s->x;
	s->height = (row == g->rows_available - 1 ? g->height : g->rows[row+1]->y) - s->y;
}

// returns FALSE if nothing has been animated
BOOL animate (struct grid *g, void (*render_function)(struct grid *))
{
	int i;

	BOOL flag = TRUE;
	while (flag)
	{
		BOOL flag2 = FALSE;
		int i;
		for (i = 0; i < g->rows_available; i++)
		{
			struct surface *s = g->rows[i];
		
			int j;
			for (j = 0; j < g->surfaces_per_row[i]; j++)
			{
				float wdif = s->target_width - s->width;
				float hdif = s->target_height - s->height;
			
				s->width = s->width + wdif * 0.1f;
				s->height = s->height + hdif * 0.1f;

				float xdif = s->target_x - s->x;
				float ydif = s->target_y - s->y;

				s->x = s->x + xdif * 0.1f;
				s->y = s->y + ydif * 0.1f;

				if((int)wdif || (int)hdif || (int)xdif || (int)ydif)
				{
					flag2 = TRUE;
				}
				
				if (s->empty_space)
					stretch_to_edges (g, s, i, j);
				s = s->next;
			}
		}

		if (flag2 == FALSE)
			flag = FALSE;
		
		render_function (g);
		
		return flag;
	}
}
