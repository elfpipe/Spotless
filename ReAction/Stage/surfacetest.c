#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "surfaces.h"

struct Window *window = NULL;

void move_drag_window();

void paint (struct grid *g)
{
    struct IntuiMessage *msg = (struct IntuiMessage *)IExec->GetMsg (window->UserPort);
    while (msg)
    {
        if (msg->Class == IDCMP_MOUSEMOVE)
            move_drag_window();
        IExec->ReplyMsg ((struct Message *)msg);
        msg = (struct IntuiMessage *) IExec->GetMsg (window->UserPort);
    }

    struct RastPort *rastport = window->RPort;
    IGraphics->RectFillColor (rastport, 1, 1, g->width, g->height, 0xff000000);
    for (int i = 0; i < g->rows_available; i++)
    {
        struct surface *s = g->rows[i];
        for (int j = 0; j < g->surfaces_per_row[i]; j++)
        {
            IGraphics->RectFillColor (rastport, s->x+1, s->y+1, s->x + s->width -2, s->y + s->height-2, s->selected ? 0xffeeffee : 0xff00ff00 );

            s = s->next;
        }
    }
    IGraphics->WaitTOF();
}

static int drag_width, drag_height;

struct Window *drag_window = NULL;

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

int main()
{
    window = IIntuition->OpenWindowTags(NULL,
        WA_Width, 1024,
        WA_Height, 768,
        WA_ReportMouse, TRUE,
        WA_IDCMP, IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_VANILLAKEY,
        TAG_DONE);

    if (window)
    {
        struct grid *g = IExec->AllocVecTags (sizeof (struct grid), TAG_DONE);

        g->rows_available = 3;

        g->width = 1024;
        g->height = 768;

        g->surfaces_per_row[0] = 3;
        g->surfaces_per_row[1] = 2;
        g->surfaces_per_row[2] = 4;

        fill_grid_with_surfaces (g);

        paint (g);

        BOOL done = FALSE;

        int insertion_row = -1;
        int insertion_index = -1;

        BOOL drag_is_in_progress = FALSE;

        while (!done)
        {
            IExec->WaitPort (window->UserPort);

            struct IntuiMessage *msg = (struct IntuiMessage *)IExec->GetMsg (window->UserPort);

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

                        if (!drag_is_in_progress)
                        {
                            if (s)
                            {
                                open_drag_window (s->width, s->height);

                                free_surface_from_row (g, row, index);
                                animate_removal_from_row (g, row, index, paint);

                                if (g->rows[row] == NULL)
                                {
                                    remove_row_from_grid (g, row);
                                    animate_removal_of_row (g, row, paint);
                                }

                                drag_is_in_progress = TRUE;
                            }
                        }
                    }
                    if (msg->Code == SELECTUP)
                    {
                        close_drag_window();

                        drag_is_in_progress = FALSE;

                        struct surface *s = surface_at_point (g, mousex, mousey, NULL, NULL);

                        if (!s)
                        {
                            if (insertion_row != -1)
                            {
                                if (insertion_index != -1)
                                {
                                    insert_grid_element_into_row (g, insertion_row, insertion_index);
                                }
                                else
                                {
                                    insert_empty_row_into_grid (g, insertion_row);
                                    struct surface *ns = insert_grid_element_into_row (g, insertion_row, 0);

                                    ns->selected = FALSE;
                                }

                                insertion_row = -1;
                                insertion_index = -1;

                                drag_is_in_progress = FALSE;
                            }
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
                        }

                        int row, index;
                        static struct surface *old_s = (struct surface *)-1;
                        struct surface *s = surface_at_point (g, mousex, mousey, &row, &index);

                        if (old_s == NULL && s)
                        {
                            if (insertion_index != -1)
                                animate_quick_closing_of_empty_space_in_row (g, row, index, paint);
                            else
                                animate_quick_closing_of_empty_row (g, row, paint);
                        }

                        old_s = s;

                        s = border_area_at_point (g, mousex, mousey, &row, &index);

                        if (s && row != -1)
                        {
                            if (index != -1)
                            {
                                if ( (insertion_row != row || insertion_index != index) )
                                {
                                    insertion_row = row;
                                    insertion_index = index;
                                    animate_grid_opening_of_empty_space_in_row (g, row, index, paint);
                                }
                            }
                            else if (insertion_row != row || insertion_index != -1)
                            {
                                insertion_row = row;
                                insertion_index = -1;
                                animate_grid_opening_of_empty_row (g, row, paint);
                            }
                        }
                    }
                    else
                    {
                        struct surface *s = surface_at_point (g, msg->MouseX, msg->MouseY, NULL, NULL);
                        if(s)
                            s->selected = TRUE;

                        paint(g);
                    }
                    break;

                    case IDCMP_VANILLAKEY:

                        done = TRUE;
                        break;

                    default:
                        break;
                }

                msg = (struct IntuiMessage *)IExec->GetMsg (window->UserPort);
            }

            paint (g);
        }

#if 0
        IDOS->Delay (100);

        animate_grid_insertion_into_row (g, 0, 2, paint);
        insert_grid_element_into_row (g, 0, 2);
         //--
        animate_grid_insertion_of_row (g, 2, paint);
        insert_empty_row_into_grid (g, 2);
        //--
        animate_grid_insertion_into_row (g, 2, 0, paint);
        insert_grid_element_into_row (g, 2, 0);
        //--
        free_surface_from_row (g, 3, 2);
        animate_removal_from_row (g, 3, 2, paint);
        //--
        remove_row_from_grid (g, 1);
        animate_removal_of_row (g, 1, paint);

        //paint (g);

        IDOS->Delay (100);
#endif

        free_all_surfaces_from_grid (g);

        IExec->FreeVec (g);

        IIntuition->CloseWindow (window);
    }

    return 0;
}

