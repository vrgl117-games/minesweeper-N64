#include <libdragon.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define WHITE graphics_make_color(0xff, 0xff, 0xff, 0xff)
#define RED graphics_make_color(0xff, 0, 0, 0xff)
#define BLACK graphics_make_color(0, 0, 0, 0xff)
#define DARK_GREY graphics_make_color(0x55, 0x55, 0x55, 0xff)
#define LIGHT_GREY graphics_make_color(0xcc, 0xcc, 0xcc, 0xff)

void graphics_draw_text_center(display_context_t disp, int x, int y, const char *const msg)
{
    // Each character is 6 pixels
    graphics_draw_text(disp, x - strlen(msg) * 6 / 2, y, msg);
}

void rdp_draw_filled_rectangle_size(int x, int y, int width, int height, uint32_t color)
{
    // Set RDP to primitive mode
    rdp_enable_primitive_fill();
    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Set color
    rdp_set_primitive_color(color);
    // Draw the rectangle
    rdp_draw_filled_rectangle(x, y, x + width, y + height);
}

sprite_t *dfs_load_sprite(const char *const path)
{
    int fp = dfs_open(path); // Open file in ROM
    if (fp > 0)
    {
        int s = dfs_size(fp);     // Get size of file
        sprite_t *sp = malloc(s); // Allocate sprite as big as file
        dfs_read(sp, 1, s, fp);   // Put content of file in sprite
        dfs_close(fp);            // Close file
        return sp;
    }
    return NULL;
}

void rdp_draw_sprite_with_texture(sprite_t *sprite, int x, int y)
{
    // Set RDP in texture mode
    rdp_enable_texture_copy();
    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Load sprite in TMEM (texture memory)
    rdp_load_texture(0, 0, MIRROR_DISABLED, sprite);
    // Draw sprite
    rdp_draw_sprite(0, x, y);
}

typedef struct
{
    int near;
    bool visible;
    bool bomb;
    bool flag;
} cell_t;

typedef struct
{
    int x;
    int y;
} cursor_t;

void draw_board(display_context_t disp, cell_t board[10][10], cursor_t cursor, sprite_t *sprites[14], bool gameover)
{
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            if (!gameover && board[x][y].flag)
                rdp_draw_sprite_with_texture(sprites[11], 60 + x * 20, 20 + y * 20);
            else if (!gameover && !board[x][y].visible)
                rdp_draw_sprite_with_texture(sprites[12], 60 + x * 20, 20 + y * 20);
            else
            {
                if (board[x][y].bomb)
                {
                    if (cursor.x == x && cursor.y == y)
                        rdp_draw_sprite_with_texture(sprites[10], 60 + x * 20, 20 + y * 20);

                    else
                        rdp_draw_sprite_with_texture(sprites[9], 60 + x * 20, 20 + y * 20);
                }
                else
                {
                    rdp_draw_sprite_with_texture(sprites[board[x][y].near], 60 + x * 20, 20 + y * 20);
                }
            }

            if (cursor.x == x && cursor.y == y)
            {
                rdp_draw_sprite_with_texture(sprites[13], 60 + x * 20, 20 + y * 20);
                //rdp_draw_filled_rectangle_size(60 + x * 20 + 4, 20 + y * 20 + 4, 20 - 8, 20 - 8, WHITE);
            }
        }
    }
}

void init_board(cell_t board[10][10])
{
    memset(board, 0, 10 * 10 * sizeof(cell_t));

    for (int i = 0; i < 8; i++)
    {
        int x = rand() % 10;
        int y = rand() % 10;

        if (board[x][y].bomb)
        {
            i--;
            continue;
        }

        if (x - 1 >= 0 && y - 1 >= 0)
            board[x - 1][y - 1].near++;
        if (y - 1 >= 0)
            board[x][y - 1].near++;
        if (x + 1 < 10 && y - 1 >= 0)
            board[x + 1][y - 1].near++;
        if (x - 1 >= 0)
            board[x - 1][y].near++;
        board[x][y].bomb = true;
        if (x + 1 < 10)
            board[x + 1][y].near++;
        if (x - 1 >= 0 && y + 1 < 10)
            board[x - 1][y + 1].near++;
        if (y + 1 < 10)
            board[x][y + 1].near++;
        if (x + 1 < 10 && y + 1 < 10)
            board[x + 1][y + 1].near++;
    }
}

void update_board(cell_t board[10][10])
{
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            if (board[x][y].visible && board[x][y].near == 0)
            {
                if (x - 1 >= 0 && y - 1 >= 0)
                    board[x - 1][y - 1].visible = true;
                if (y - 1 >= 0)
                    board[x][y - 1].visible = true;
                if (x + 1 < 10 && y - 1 >= 0)
                    board[x + 1][y - 1].visible = true;
                if (x - 1 >= 0)
                    board[x - 1][y].visible = true;
                if (x + 1 < 10)
                    board[x + 1][y].visible = true;
                if (x - 1 >= 0 && y + 1 < 10)
                    board[x - 1][y + 1].visible = true;
                if (y + 1 < 10)
                    board[x][y + 1].visible = true;
                if (x + 1 < 10 && y + 1 < 10)
                    board[x + 1][y + 1].visible = true;
            }
        }
    }
}

int main()
{
    init_interrupts();

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    display_context_t disp = 0;
    dfs_init(DFS_DEFAULT_LOCATION);
    rdp_init();

    // Remove any clipping windows
    rdp_set_default_clipping();

    controller_init();

    timer_init();
    srand(timer_ticks());

    cell_t board[10][10];
    cursor_t cursor = {0};
    bool gameover = false;

    sprite_t *sprites[14];
    sprites[0] = dfs_load_sprite("/gfx/0.sprite");
    sprites[1] = dfs_load_sprite("/gfx/1.sprite");
    sprites[2] = dfs_load_sprite("/gfx/2.sprite");
    sprites[3] = dfs_load_sprite("/gfx/3.sprite");
    sprites[4] = dfs_load_sprite("/gfx/4.sprite");
    sprites[5] = dfs_load_sprite("/gfx/5.sprite");
    sprites[6] = dfs_load_sprite("/gfx/6.sprite");
    sprites[7] = dfs_load_sprite("/gfx/7.sprite");
    sprites[8] = dfs_load_sprite("/gfx/8.sprite");
    sprites[9] = dfs_load_sprite("/gfx/bomb.sprite");
    sprites[10] = dfs_load_sprite("/gfx/bomb_hit.sprite");
    sprites[11] = dfs_load_sprite("/gfx/flag.sprite");
    sprites[12] = dfs_load_sprite("/gfx/hidden.sprite");
    sprites[13] = dfs_load_sprite("/gfx/cursor.sprite");

    init_board(board);

    while (true)
    {
        controller_scan();

        struct controller_data keys = get_keys_down();
        if (!gameover)
        {
            if (keys.c[0].right)
                cursor.x = (cursor.x + 1) % 10;
            if (keys.c[0].left)
                cursor.x = (cursor.x == 0 ? 9 : cursor.x - 1);
            if (keys.c[0].down)
                cursor.y = (cursor.y + 1) % 10;
            if (keys.c[0].up)
                cursor.y = (cursor.y == 0 ? 9 : cursor.y - 1);
        }

        // update game state
        if (keys.c[0].A)
        {
            board[cursor.x][cursor.y].visible = true;
            if (board[cursor.x][cursor.y].bomb)
                gameover = true;
        }
        else if (keys.c[0].B)
        {
            board[cursor.x][cursor.y].flag = true;
        }
        else if (keys.c[0].start)
        {
            init_board(board);
            gameover = false;
        }

        update_board(board);

        while (!(disp = display_lock()))
            ;

        rdp_attach_display(disp);

        if (gameover)
            rdp_draw_filled_rectangle_size(0, 0, 320, 240, RED);
        else
            rdp_draw_filled_rectangle_size(0, 0, 320, 240, WHITE);

        draw_board(disp, board, cursor, sprites, gameover);

        rdp_detach_display();

        if (gameover)
        {
            graphics_set_color(WHITE, BLACK);
            graphics_draw_text_center(disp, 160, 120, "GAME OVER\n");
        }
        else
        {
            graphics_set_color(BLACK, 0);
        }

        graphics_draw_text_center(disp, 160, 5, "MINESWEEPER 64");

        display_show(disp);
    }

    return 0;
}
