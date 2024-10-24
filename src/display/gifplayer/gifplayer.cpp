#include "vex.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct Entry
{
    uint16_t length;
    uint16_t prefix;
    uint8_t suffix;
} Entry;

typedef struct Table
{
    int bulk;
    int nentries;
    Entry *entries;
} Table;

static uint16_t
read_num(int fd)
{
    uint8_t bytes[2];

    read(fd, bytes, 2);
    return bytes[0] + (((uint16_t)bytes[1]) << 8);
}

gd_GIF *
gd_open_gif(const char *fname)
{
    int fd;
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int i;
    const uint8_t *bgcolor;
    int gct_sz;
    gd_GIF *gif;

    fd = open(fname, O_RDONLY);
    if (fd == -1)
        return nullptr;
    /* Header */
    read(fd, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0)
    {
        logHandler("gd_open_gif (Extern)", "Invalid gif signature.", Log::Level::Error, 3);
        goto fail;
    }
    /* Version */
    read(fd, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0)
    {
        logHandler("gd_open_gif (Extern)", "Invalid gif version.", Log::Level::Error, 3);
        goto fail;
    }
    /* Width x Height */
    width = read_num(fd);
    height = read_num(fd);
    /* FDSZ */
    read(fd, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80))
    {
        logHandler("gd_open_gif (Extern)", "No color table for gif file detected.", Log::Level::Error, 5);
        goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    read(fd, &bgidx, 1);
    /* Aspect Ratio */
    read(fd, &aspect, 1);
    /* Create gd_GIF Structure. */
    gif = static_cast<gd_GIF *>(calloc(1, sizeof(*gif)));
    if (!gif)
        goto fail;
    gif->fd = fd;
    gif->width = width;
    gif->height = height;
    gif->depth = depth;
    /* Read GCT */
    gif->gct.size = gct_sz;
    read(fd, gif->gct.colors, 3 * gif->gct.size);
    gif->palette = &gif->gct;
    gif->bgindex = bgidx;
    gif->frame = static_cast<uint8_t *>(calloc(4, width * height));
    if (!gif->frame)
    {
        free(gif);
        goto fail;
    }
    gif->canvas = &gif->frame[width * height];
    if (gif->bgindex)
        memset(gif->frame, gif->bgindex, gif->width * gif->height);
    bgcolor = &gif->palette->colors[gif->bgindex * 3];
    if (bgcolor[0] || bgcolor[1] || bgcolor[2])
        for (i = 0; i < gif->width * gif->height; i++)
            memcpy(&gif->canvas[i * 3], bgcolor, 3);
    gif->anim_start = lseek(fd, 0, SEEK_CUR);
    goto ok;
fail:
    close(fd);
    return 0;
ok:
    return gif;
}

static void
discard_sub_blocks(gd_GIF *gif)
{
    uint8_t size;

    do
    {
        read(gif->fd, &size, 1);
        lseek(gif->fd, size, SEEK_CUR);
    } while (size);
}

static void
read_plain_text_ext(gd_GIF *gif)
{
    if (gif->plain_text)
    {
        uint16_t tx, ty, tw, th;
        uint8_t cw, ch, fg, bg;
        off_t sub_block;
        lseek(gif->fd, 1, SEEK_CUR); /* block size = 12 */
        tx = read_num(gif->fd);
        ty = read_num(gif->fd);
        tw = read_num(gif->fd);
        th = read_num(gif->fd);
        read(gif->fd, &cw, 1);
        read(gif->fd, &ch, 1);
        read(gif->fd, &fg, 1);
        read(gif->fd, &bg, 1);
        sub_block = lseek(gif->fd, 0, SEEK_CUR);
        gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
        lseek(gif->fd, sub_block, SEEK_SET);
    }
    else
    {
        /* Discard plain text metadata. */
        lseek(gif->fd, 13, SEEK_CUR);
    }
    /* Discard plain text sub-blocks. */
    discard_sub_blocks(gif);
}

static void
read_graphic_control_ext(gd_GIF *gif)
{
    uint8_t rdit;

    /* Discard block size (always 0x04). */
    lseek(gif->fd, 1, SEEK_CUR);
    read(gif->fd, &rdit, 1);
    gif->gce.disposal = (rdit >> 2) & 3;
    gif->gce.input = rdit & 2;
    gif->gce.transparency = rdit & 1;
    gif->gce.delay = read_num(gif->fd);
    read(gif->fd, &gif->gce.tindex, 1);
    /* Skip block terminator. */
    lseek(gif->fd, 1, SEEK_CUR);
}

static void
read_comment_ext(gd_GIF *gif)
{
    if (gif->comment)
    {
        off_t sub_block = lseek(gif->fd, 0, SEEK_CUR);
        gif->comment(gif);
        lseek(gif->fd, sub_block, SEEK_SET);
    }
    /* Discard comment sub-blocks. */
    discard_sub_blocks(gif);
}

static void
read_application_ext(gd_GIF *gif)
{
    char app_id[8];
    char app_auth_code[3];

    /* Discard block size (always 0x0B). */
    lseek(gif->fd, 1, SEEK_CUR);
    /* Application Identifier. */
    read(gif->fd, app_id, 8);
    /* Application Authentication Code. */
    read(gif->fd, app_auth_code, 3);
    if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
    {
        /* Discard block size (0x03) and constant byte (0x01). */
        lseek(gif->fd, 2, SEEK_CUR);
        gif->loop_count = read_num(gif->fd);
        /* Skip block terminator. */
        lseek(gif->fd, 1, SEEK_CUR);
    }
    else if (gif->application)
    {
        off_t sub_block = lseek(gif->fd, 0, SEEK_CUR);
        gif->application(gif, app_id, app_auth_code);
        lseek(gif->fd, sub_block, SEEK_SET);
        discard_sub_blocks(gif);
    }
    else
    {
        discard_sub_blocks(gif);
    }
}

static void
read_ext(gd_GIF *gif)
{
    uint8_t label;

    read(gif->fd, &label, 1);
    switch (label)
    {
    case 0x01:
        read_plain_text_ext(gif);
        break;
    case 0xF9:
        read_graphic_control_ext(gif);
        break;
    case 0xFE:
        read_comment_ext(gif);
        break;
    case 0xFF:
        read_application_ext(gif);
        break;
    default:
        logHandler("gd_open_gif (Extern)", "Unknown Gif extention: %02X" + std::to_string(label), Log::Level::Error, 3);
    }
}

static Table *
new_table(int key_size)
{
    int init_bulk = MAX(1 << (key_size + 1), 0x100);
    Table *table = static_cast<Table *>(malloc(sizeof(*table) + sizeof(Entry) * init_bulk));
    if (table)
    {
        table->bulk = init_bulk;
        table->nentries = (1 << key_size) + 2;
        table->entries = reinterpret_cast<Entry *>(&table[1]);
        for (int key = 0; key < (1 << key_size); key++)
            table->entries[key] = (Entry){1, 0xFFF, static_cast<uint8_t>(key)};
    }
    return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int
add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix)
{
    Table *table = *tablep;
    if (table->nentries == table->bulk)
    {
        table->bulk *= 2;
        table = static_cast<Table *>(realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk));
        if (!table)
            return -1;
        table->entries = reinterpret_cast<Entry *>(&table[1]);
        *tablep = table;
    }
    table->entries = reinterpret_cast<Entry *>(&table[1]);
    table->entries[table->nentries] = (Entry){length, prefix, suffix};
    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0)
        return 1;
    return 0;
}

static uint16_t
get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
{
    int bits_read;
    int rpad;
    int frag_size;
    uint16_t key;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size)
    {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0)
        {
            /* Update byte. */
            if (*sub_len == 0)
            {
                read(gif->fd, sub_len, 1); /* Must be nonzero! */
                if (*sub_len == 0)
                    return 0x1000;
            }
            read(gif->fd, byte, 1);
            (*sub_len)--;
        }
        frag_size = MIN(key_size - bits_read, 8 - rpad);
        key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int
interlaced_line_index(int h, int y)
{
    int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
        return y * 8;
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
        return y * 8 + 4;
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
        return y * 4 + 2;
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image_data(gd_GIF *gif, int interlace)
{
    uint8_t sub_len, shift, byte;
    int init_key_size, key_size, table_is_full;
    int frm_off, frm_size, str_len, i, p, x, y;
    uint16_t key, clear, stop;
    int ret;
    Table *table;
    Entry entry;
    off_t start, end;

    read(gif->fd, &byte, 1);
    key_size = (int)byte;
    if (key_size < 2 || key_size > 8)
        return -1;

    start = lseek(gif->fd, 0, SEEK_CUR);
    discard_sub_blocks(gif);
    end = lseek(gif->fd, 0, SEEK_CUR);
    lseek(gif->fd, start, SEEK_SET);
    clear = 1 << key_size;
    stop = clear + 1;
    table = new_table(key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    frm_size = gif->fw * gif->fh;
    while (frm_off < frm_size)
    {
        if (key == clear)
        {
            key_size = init_key_size;
            table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        }
        else if (!table_is_full)
        {
            ret = add_entry(&table, str_len + 1, key, entry.suffix);
            if (ret == -1)
            {
                free(table);
                return -1;
            }
            if (table->nentries == 0x1000)
            {
                ret = 0;
                table_is_full = 1;
            }
        }
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear)
            continue;
        if (key == stop || key == 0x1000)
            break;
        if (ret == 1)
            key_size++;
        entry = table->entries[key];
        str_len = entry.length;
        for (i = 0; i < str_len; i++)
        {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace)
                y = interlaced_line_index((int)gif->fh, y);
            gif->frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            if (entry.prefix == 0xFFF)
                break;
            else
                entry = table->entries[entry.prefix];
        }
        frm_off += str_len;
        if (key < table->nentries - 1 && !table_is_full)
            table->entries[table->nentries - 1].suffix = entry.suffix;
    }
    free(table);
    if (key == stop)
        read(gif->fd, &sub_len, 1); /* Must be zero! */
    lseek(gif->fd, end, SEEK_SET);
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image(gd_GIF *gif)
{
    uint8_t fisrz;
    int interlace;

    /* Image Descriptor. */
    gif->fx = read_num(gif->fd);
    gif->fy = read_num(gif->fd);

    if (gif->fx >= gif->width || gif->fy >= gif->height)
        return -1;

    gif->fw = read_num(gif->fd);
    gif->fh = read_num(gif->fd);

    gif->fw = MIN(gif->fw, gif->width - gif->fx);
    gif->fh = MIN(gif->fh, gif->height - gif->fy);

    read(gif->fd, &fisrz, 1);
    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80)
    {
        /* Read LCT */
        gif->lct.size = 1 << ((fisrz & 0x07) + 1);
        read(gif->fd, gif->lct.colors, 3 * gif->lct.size);
        gif->palette = &gif->lct;
    }
    else
        gif->palette = &gif->gct;
    /* Image Data. */
    return read_image_data(gif, interlace);
}

static void
render_frame_rect(gd_GIF *gif, uint8_t *buffer)
{
    int i, j, k;
    uint8_t index;
    const uint8_t *color;
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++)
    {
        for (k = 0; k < gif->fw; k++)
        {
            index = gif->frame[(gif->fy + j) * gif->width + gif->fx + k];
            color = &gif->palette->colors[index * 3];
            if (!gif->gce.transparency || index != gif->gce.tindex)
                memcpy(&buffer[(i + k) * 3], color, 3);
        }
        i += gif->width;
    }
}

static void
dispose(gd_GIF *gif)
{
    int i, j, k;
    const uint8_t *bgcolor;
    switch (gif->gce.disposal)
    {
    case 2: /* Restore to background color. */
        bgcolor = &gif->palette->colors[gif->bgindex * 3];
        i = gif->fy * gif->width + gif->fx;
        for (j = 0; j < gif->fh; j++)
        {
            for (k = 0; k < gif->fw; k++)
                memcpy(&gif->canvas[(i + k) * 3], bgcolor, 3);
            i += gif->width;
        }
        break;
    case 3: /* Restore to previous, i.e., don't update canvas.*/
        break;
    default:
        /* Add frame non-transparent pixels to canvas. */
        render_frame_rect(gif, gif->canvas);
    }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
int gd_get_frame(gd_GIF *gif)
{
    char sep;

    dispose(gif);
    read(gif->fd, &sep, 1);
    while (sep != ',')
    {
        if (sep == ';')
            return 0;
        if (sep == '!')
            read_ext(gif);
        else
            return -1;
        read(gif->fd, &sep, 1);
    }
    if (read_image(gif) == -1)
        return -1;
    return 1;
}

void gd_render_frame(gd_GIF *gif, uint8_t *buffer)
{
    memcpy(buffer, gif->canvas, gif->width * gif->height * 3);
    render_frame_rect(gif, buffer);
}

int gd_is_bgcolor(const gd_GIF *gif, const uint8_t color[3])
{
    return !memcmp(&gif->palette->colors[gif->bgindex * 3], color, 3);
}

void gd_rewind(gd_GIF *gif)
{
    lseek(gif->fd, gif->anim_start, SEEK_SET);
}

void gd_close_gif(gd_GIF *gif)
{
    close(gif->fd);
    free(gif->frame);
    free(gif);
}

/*----------------------------------------------------------------------------*/
// C++ wrapper class by James
// 2019 - 2022
// MIT license
//

// static member function to handle rendering frames
//
int vex::Gif::render_task(void *arg)
{
    if (arg == nullptr)
        return (0);

    Gif *instance = static_cast<Gif *>(arg);

    gd_GIF *gif = instance->_gif;

    for (unsigned looped = 1;; looped++)
    {
        int32_t now = instance->_timer.system();
        int err = 0;
        instance->_frame = 0;

        while ((err = gd_get_frame(gif)) > 0)
        {
            gd_render_frame(gif, static_cast<uint8_t *>(instance->_buffer));

            instance->_lcd.drawImageFromBuffer(static_cast<uint32_t *>(instance->_buffer), instance->_sx, instance->_sy, gif->width, gif->height);
            instance->_frame++;

            // how long to get, render and draw to screen
            int32_t delay = gif->gce.delay * 10;

            // do we need delay to honor loop speed
            int32_t delta = instance->_timer.system() - now;
            delay -= delta;
            if (delay > 0)
            {
                this_thread::sleep_for(delay);
            }

            // for next loop
            now = instance->_timer.system();
        }
        if (err == -1)
        {
            logHandler("vex::Gif::render_task (Extern)", "Unknown gif Error.", Log::Level::Error, 3);
            break;
        }
        // done ?
        if (looped == gif->loop_count)
        {
            break;
        }

        gd_rewind(gif);
    }

    instance->cleanup();

    return (0);
}

vex::Gif::Gif(const char *fname, int sx, int sy, bool bMemoryBuffer)
{
    _sx = sx;
    _sy = sy;
    int fd = open(fname, O_RDONLY);

    if (fd != -1)
    {
        // get file length
        struct stat st;
        fstat(fd, &st);

        // optimize by reading into memory buffer ?
        // using memory, then reopen file
        if (bMemoryBuffer)
        {
            size_t len = st.st_size;
            _gifmem = malloc(len);
            if (_gifmem != nullptr)
            {
                read(fd, _gifmem, len);
                close(fd);
                fd = open(fname, O_RDONLY);
            }
        }

        // good file ?
        if (fd != -1)
        {
            // open gif file
            // will allocate memory for background and one animation
            // frame.
            _gif = gd_open_gif(fname);
            if (_gif == nullptr)
            {
                return;
            }

            // memory for rendering frame
            _buffer = static_cast<uint32_t *>(malloc(_gif->width * _gif->height * sizeof(uint32_t)));
            if (_buffer == nullptr)
            {
                // out of memory
                gd_close_gif(_gif);
                if (_gifmem)
                {
                    free(_gifmem);
                }
            }
            else
            {
                // create thread to handle this gif
                _t1 = thread(render_task, static_cast<void *>(this));
            }
        }
    }
};

vex::Gif::~Gif()
{
    cleanup();
};

// cleanup memory when gif finishes
//

void vex::Gif::cleanup()
{
    // stop thread
    _t1.interrupt();

    if (_buffer)
    {
        free(_buffer);
        _buffer = nullptr;
    }

    if (_gif)
    {
        gd_close_gif(_gif);
        _gif = nullptr;
    }

    if (_gifmem)
    {
        free(_gifmem);
        _gifmem = nullptr;
    }
}

// get current rendered frame
//

int vex::Gif::getFrameIndex()
{
    return _frame;
}