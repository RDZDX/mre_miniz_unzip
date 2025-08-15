#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "mre_stdlibio.h"
#include "stdint.h"

#include "mre_stdlibio.h"
#include "miniminiz.h"

VMINT layer_hdl[1];				////layer handle array.

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
static void fill_white(void);
VMINT string_width(VMWCHAR* whead, VMWCHAR* wtail);
void display_text_line(VMUINT8* disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos);
VMINT job(VMWCHAR *file_path, VMINT wlen);
void remove_file_extension_wchar(VMWCHAR *filename);
void normalize_path_slashes(char *path);
void mkdir_recursive_ascii(const char *path);
static VMINT safe_vm_file_write(VMFILE fh, const void *buf, VMUINT len);
int unzip_all_files_to_folder_stream_diag(VMWSTR input_path);
void _sbrk();
void _write();
void _close();
void _lseek();
void _open();
void _read();
void _exit(int status);
void _getpid();
void _kill();
void _fstat();
void _isatty();

#endif


