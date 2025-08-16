#include "mre_miniz_unzip.h"

#define ZIP_EXTRACT_BUF_SIZE 4096

VMUINT8* screenbuf = NULL;
VMINT ncharacter_height = -1;
VMINT nscreen_width = -1;
VMINT nscreen_height = -1;
VMINT filledDsplByLines = 0;
VMBOOL third = VM_FALSE;
VMCHAR text[256] = {0};
VMWCHAR defaultFullPathName[100];
VMWCHAR autoFullPathName[100];
char debug_buf[256] = {0};
VMWCHAR file_pathk[260] = {0};
static VMINT timer_id = -1;

VMINT response = -1;
VMINT trigeris = 0;
VMINT trigeris1 = 0;
VMINT trigeris2 = 0;
VMBOOL second = VM_FALSE;

void _sbrk(){}
void _write(){}
void _close(){}
void _lseek(){}
void _open(){}
void _read(){}
void _exit(int status) {
	vm_exit_app();
}
void _getpid(){}
void _kill(){}
void _fstat(){}
void _isatty(){}

void vm_main(void) {

        layer_hdl[0] = -1;
	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_font_set_font_size(VM_SMALL_FONT);
	ncharacter_height = vm_graphic_get_character_height();
	nscreen_width = vm_graphic_get_screen_width();
	nscreen_height = vm_graphic_get_screen_height();
        response = vm_selector_run(0, 0, job);
}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), -1);
            vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
            //fill_white();
            break;

        case VM_MSG_PAINT:
            vm_switch_power_saving_mode(turn_off_mode);
            screenbuf = vm_graphic_get_layer_buffer(layer_hdl[0]);

            if (trigeris != 0) {vm_exit_app();}
            if (trigeris1 == 1) {trigeris2 = 1;}
            if (response == 0) {trigeris1 = 1;}
            if (trigeris2 == 1) {vm_exit_app();}
            if(second == VM_TRUE){
              second = VM_FALSE;
              unzip_all_files_to_folder_stream_diag(file_pathk);
            } else {
              fill_white();
            }
            break;

        case VM_MSG_INACTIVE:
            vm_switch_power_saving_mode(turn_on_mode);
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);
            break;

        case VM_MSG_QUIT:
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);
            break;
    }
}

void handle_keyevt(VMINT event, VMINT keycode) {

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
        if (layer_hdl[0] != -1) {
            vm_graphic_delete_layer(layer_hdl[0]);
            layer_hdl[0] = -1;
        }
        vm_exit_app();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
        second = VM_FALSE;
        vm_selector_run(0, 0, job);
    }


}

VMINT job(VMWCHAR *file_path, VMINT wlen) {

    response = 1;
    //trigeris = 1;
    trigeris1 = 0;
    trigeris2 = 0;
    filledDsplByLines = 0;

    vm_wstrcpy(file_pathk, file_path);

    second = VM_TRUE;

    return 0;

}

static void fill_white(void) {

    vm_graphic_color color;

    color.vm_color_565 = VM_COLOR_WHITE;
    vm_graphic_setcolor(&color);
    vm_graphic_fill_rect_ex(layer_hdl[0], 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
    vm_graphic_flush_layer(layer_hdl, 1);
    filledDsplByLines = 0;
}

VMINT string_width(VMWCHAR* whead, VMWCHAR* wtail) {

    VMWCHAR* wtemp = NULL;
    VMINT width = 0;
    if (whead == NULL || wtail == NULL) return 0;
    wtemp = (VMWCHAR*)vm_malloc((wtail - whead) * 2 + 2);

    if (wtemp == NULL) return 0;
    memset(wtemp, 0, (wtail - whead) * 2 + 2);
    memcpy(wtemp, whead, (wtail - whead) * 2);

    width = vm_graphic_get_string_width(wtemp);
    vm_free(wtemp);
    return width;
}

void display_text_line(VMUINT8* disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos) {

    VMWCHAR* ucstr;
    VMWCHAR* ucshead;
    VMWCHAR* ucstail;
    VMINT is_end = FALSE;
    VMINT nheight = y;
    VMINT nline_height;
    VMINT nlines = 0;

    if (y == 0) {
        fill_white();
    }

    if (str == NULL || disp_buf == NULL || betlines < 0) {
        return;
    }

    nline_height = vm_graphic_get_character_height() + betlines;

    if (third == VM_TRUE && fix_pos == VM_FALSE) {
        nheight = nheight + nline_height;
        third = VM_FALSE;
    }

    if (fix_pos == VM_TRUE) {
        vm_graphic_fill_rect(screenbuf, 0, filledDsplByLines, nscreen_width,
                             nline_height, VM_COLOR_WHITE, VM_COLOR_WHITE);
        third = VM_TRUE;
    }

    ucstr = (VMWCHAR*)vm_malloc(2 * (strlen(str) + 1));

    if (ucstr == NULL) {
        return;
    }

    if (0 != vm_ascii_to_ucs2(ucstr, 2 * (strlen(str) + 1), str)) {
        vm_free(ucstr);
        return;
    }
    ucshead = ucstr;
    ucstail = ucshead + 1;

    while (is_end == FALSE) {
 
        if (nheight > y + height) {  // jeigu uzimtas ekrano aukstis virsijo 320
            fill_white();
            nheight = 0;

        }

        while (1) {
            if (string_width(ucshead, ucstail) <= width) {
                ucstail++;
            } else {
                nlines++;
                ucstail--;
                break;
            }
            if (0 == vm_wstrlen(ucstail)) {
                is_end = TRUE;
                nlines++;
                break;
            }
        }
        if (nlines >= startLine) {
            vm_graphic_textout(disp_buf, x, nheight, ucshead,
                               (ucstail - ucshead), (VMUINT16)(color));
            vm_graphic_flush_layer(layer_hdl, 1);
            if (fix_pos == VM_FALSE) {
                nheight += nline_height;
            }
            filledDsplByLines = nheight;
        }
        ucshead = ucstail;
        ucstail++;
    }
    vm_free(ucstr);
    vm_graphic_flush_layer(layer_hdl, 1);
}

void remove_file_extension_wchar(VMWCHAR *filename) {

    int len = 0;
    int dot_pos = -1;

    // Surasti paskutinio taško (.) poziciją
    while (filename[len] != 0) {
        if (filename[len] == (VMWCHAR)'.') {
            dot_pos = len;
        }
        len++;
    }

    // Jei rasta, nutraukti eilutę ties tašku
    if (dot_pos != -1) {
        filename[dot_pos] = 0;
    }
}

void normalize_path_slashes(char *path) {

    int i;

    for (i = 0; path[i]; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }
}

void mkdir_recursive_ascii(const char *path) {

    char partial[260];
    int len = strlen(path);
    int i, j = 0;

    for (i = 0; i <= len; i++) {
        if (path[i] == '\\' || path[i] == '/' || path[i] == '\0') {
            if (j > 0) {
                partial[j] = '\0';
                VMWCHAR wtemp[260];
                vm_ascii_to_ucs2(wtemp, sizeof(wtemp), partial);
                vm_file_mkdir(wtemp);
            }
            partial[j++] = path[i];
        } else {
            partial[j++] = path[i];
        }
    }
}

static VMINT safe_vm_file_write(VMFILE fh, const void *buf, VMUINT len) {

    const VMUINT8 *p = (const VMUINT8 *)buf;
    VMUINT done = 0;
    while (done < len) {
        VMUINT wrote = 0;
        VMINT rc = vm_file_write(fh, (void *)(p + done), (VMUINT)(len - done), &wrote);
        if (rc < 0 || wrote == 0) return -1;
        done += wrote;
    }
    return 0;
}

int unzip_all_files_to_folder_stream_diag(VMWSTR input_path) {

    VMWCHAR wpath[260], out_filename_w[260], zip_name_w[260];
    char folder_ascii[260], root_folder_ascii[260], out_ascii[260];
    mz_zip_archive zip_archive;
    int num_files, i;
    char debug_buf[256];
    VMINT rc;

    sprintf(debug_buf, "Extracting...");
    display_text_line(screenbuf, debug_buf, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

    // Gauti katalogo kelią, kuriame yra ZIP failas
    vm_get_path(input_path, wpath);
    vm_ucs2_to_ascii(folder_ascii, sizeof(folder_ascii), wpath);

    // Gauti ZIP failo vardą be plėtinio ir sukurti šakninį katalogą
    vm_get_filename(input_path, zip_name_w);      // pvz. "testz1.zip"
    remove_file_extension_wchar(zip_name_w);      // pvz. "testz1"
    vm_wstrcat(wpath, zip_name_w);                 // pvz. "E:\\testz1"
    vm_file_mkdir(wpath);                          // Sukuriam root katalogą
    vm_ucs2_to_ascii(root_folder_ascii, sizeof(root_folder_ascii), wpath);

    memset(&zip_archive, 0, sizeof(zip_archive));
    char zip_ascii[260];
    vm_ucs2_to_ascii(zip_ascii, sizeof(zip_ascii), input_path);

    rc = mz_zip_reader_init_file(&zip_archive, zip_ascii, 0);

    if (!rc) {
        return -1;
    }

    num_files = (int)mz_zip_reader_get_num_files(&zip_archive);

    for (i = 0; i < num_files; i++) {
        mz_zip_archive_file_stat st;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &st)) continue;

        // Pilnas kelias: root_folder + st.m_filename
        strcpy(out_ascii, root_folder_ascii);
        strcat(out_ascii, "\\");
        strcat(out_ascii, st.m_filename);
        normalize_path_slashes(out_ascii);

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            mkdir_recursive_ascii(out_ascii);
            continue;
        }

        // Užtikrinti, kad tėviniai katalogai egzistuoja
        char tmp_dir[260];
        strcpy(tmp_dir, out_ascii);
        char *slash = strrchr(tmp_dir, '\\');
        if (slash) {
            *slash = '\0';
            mkdir_recursive_ascii(tmp_dir);
        }

        mz_zip_reader_extract_iter_state *iter = mz_zip_reader_extract_iter_new(&zip_archive, i, 0);
        if (!iter) continue;

       sprintf(debug_buf, out_ascii);
       display_text_line(screenbuf, debug_buf, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

        vm_ascii_to_ucs2(out_filename_w, sizeof(out_filename_w), out_ascii);
        VMFILE out_fh = vm_file_open(out_filename_w, MODE_CREATE_ALWAYS_WRITE, TRUE);
        if (out_fh < 0) {
            mz_zip_reader_extract_iter_free(iter);
            continue;
        }

        uint8_t buf[ZIP_EXTRACT_BUF_SIZE];
        unsigned total_written = 0;
        while (total_written < st.m_uncomp_size) {
            size_t to_read = sizeof(buf);
            size_t n = mz_zip_reader_extract_iter_read(iter, buf, to_read);
            if (n == 0) break;
            if (safe_vm_file_write(out_fh, buf, (VMUINT)n) < 0) break;
            total_written += n;
        }

        vm_file_close(out_fh);

        mz_zip_reader_extract_iter_free(iter);

    }

    mz_zip_reader_end(&zip_archive);
    sprintf(debug_buf, "Done");
    display_text_line(screenbuf, debug_buf, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

    if (zip_archive.m_pState && zip_archive.m_pState->m_pFile) {
        VMFILE zip_fh = *(VMFILE *)zip_archive.m_pState->m_pFile;
        if (zip_fh >= 0) {
            vm_file_close(zip_fh);
        }
        zip_archive.m_pState->m_pFile = NULL;
    }

    return 0;
}
