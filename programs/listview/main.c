/*
 *  Listview Demo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#include "listview_res.h"
#include "wine/unicode.h"

HINSTANCE hInst;
HWND     hMainWnd;
HWND     hList;

static LRESULT WINAPI LISTVIEW_WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                               LPARAM lParam)
{
  //printf("Message: %x\n", msg);
  LPNMHDR lpnmh;
    switch (msg) {

    case WM_CREATE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        static const WCHAR listviewW[] = { 'S','y','s','L','i','s','t','V','i','e','w','3','2',0 };
        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_EDITLABELS | LVS_REPORT | LVS_NOCOLUMNHEADER;
        hList = CreateWindowExW(WS_EX_CLIENTEDGE, listviewW, NULL,
                             dwStyle, 0, 0, rc.right, rc.bottom, hWnd,
                             NULL, hInst, NULL);

        SendMessageA(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SUBITEMIMAGES);

        HICON hiconItem;
        HIMAGELIST hSmall;
        hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
                                  GetSystemMetrics(SM_CYSMICON),
                                  ILC_MASK, 1, 1);
        hiconItem = LoadIconA(hInst, MAKEINTRESOURCE(IDI_LISTVIEW));
        ImageList_AddIcon(hSmall, hiconItem);
        DestroyIcon(hiconItem);
        SendMessageA(hList, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hSmall);

        LVCOLUMNA col;
        ZeroMemory(&col, sizeof(col));
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        col.cx = 80;

        col.pszText="Item";
        col.iSubItem = 0;
        SendMessageA(hList, LVM_INSERTCOLUMNA, 0, (LPARAM)&col);
        col.pszText="Visible";
        col.iSubItem = 1;
        SendMessageA(hList, LVM_INSERTCOLUMNA, 1, (LPARAM)&col);
        col.pszText="Color";
        col.iSubItem = 2;
        SendMessageA(hList, LVM_INSERTCOLUMNA, 2, (LPARAM)&col);

        static INT order[] = {1, 2, 0};
        SendMessageA(hList, LVM_SETCOLUMNORDERARRAY, sizeof(order) / sizeof(INT), (LPARAM)&order);

        for (int i = 0; i < 4; ++i) {
          LVITEMA item;
          ZeroMemory(&item, sizeof(item));
          item.cchTextMax = 256;

          item.mask = LVIF_TEXT;
          item.iItem = i;
          item.iSubItem = 0;
          char buf[200];
          sprintf(buf, "Item %d", i);
          item.pszText = buf;
          SendMessageA(hList, LVM_INSERTITEMA, 0, (LPARAM)&item);

          item.mask = LVIF_IMAGE;
          item.iSubItem = 1;
          item.pszText = "";
          item.iImage = 0;
          SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&item);

          item.mask = LVIF_IMAGE;
          item.iSubItem = 2;
          item.pszText = "";
          item.iImage = 0;
          SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&item);
        }

        break;
    }

    case WM_NOTIFY:
      //printf("Notify: %x\n", ((LPNMHDR)lParam)->code);
      lpnmh = (LPNMHDR)lParam;
      if (lpnmh->hwndFrom == hList) {
        if (lpnmh->code == NM_CLICK) {
          LPNMITEMACTIVATE lpact = (LPNMITEMACTIVATE)lParam;
          printf("Click on list: item: %d subitem: %d\n", lpact->iItem, lpact->iSubItem);
          if (lpact->iSubItem > 0 && lpact->iItem >= 0) {
            LVITEMA item;
            ZeroMemory(&item, sizeof(item));
            item.cchTextMax = 256;
            item.mask = LVIF_TEXT;
            item.iItem = lpact->iItem;
            item.iSubItem = lpact->iSubItem;
            item.pszText = "Toggle";
            item.iImage = 0;
            SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&item);
          }
        }
      }
        break;

    case WM_CLOSE:
      DestroyWindow(hWnd);
      break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
        break;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

/***********************************************************************
 *
 *           WinMain
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmdline, int show)
{
    MSG msg;
    WNDCLASSEXW class;
    static const WCHAR className[] = {'L','i','s','t','v','i','e','w',0};
    static const WCHAR winName[]   = {'L','i','s','t','v','i','e','w',0};

    hInst = hInstance;

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
    InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
    if (!InitCommonControlsEx(&InitCtrls)) {
      return FALSE;
    }

    ZeroMemory(&class, sizeof(class));
    class.cbSize        = sizeof(class);
    class.lpfnWndProc   = LISTVIEW_WndProc;
    class.hInstance     = hInstance;
    class.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_LISTVIEW));
    class.hIconSm       = LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_LISTVIEW), IMAGE_ICON,
                                     GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                     LR_SHARED);
    class.hCursor       = LoadCursorW(0, (LPCWSTR)IDC_ARROW);
    class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    class.lpszClassName = className;

    if (!RegisterClassExW(&class)) return FALSE;

    hMainWnd =
      CreateWindowW(className, winName, WS_OVERLAPPEDWINDOW, 0, 0, 500, 500,
                      NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
    {
        ExitProcess(1);
    }

    ShowWindow(hMainWnd, show);
    UpdateWindow(hMainWnd);

    while (GetMessageW(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return msg.wParam;
}
