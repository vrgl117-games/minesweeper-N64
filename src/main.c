#include <libdragon.h>

int main()
{
    init_interrupts();

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    display_context_t disp = 0;
   
    rdp_init();

    while (true)
    {
        // get controllers
        // update game state
        while (!(disp = display_lock()));
        // update display
        display_show(disp);
    }

    return 0;
}
