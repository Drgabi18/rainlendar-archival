/*
  Copyright (C) 2004 Kimmo Pekkola

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginOutlook/SetupDialog.cpp,v 1.2 2005/09/08 16:09:12 rainy Exp $

  $Log: SetupDialog.cpp,v $
  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/01 18:33:11  rainy
  Added possibility to select which message stores are imported.
  No direct linking to mapi32.dll anymore.

  Revision 1.2  2004/12/05 18:11:17  rainy
  Added NoMessageBody and NoDoneTasks

  Revision 1.1  2004/11/06 13:32:13  rainy
  Initial version

*/

#pragma warning(disable: 4786)

#include <windows.h>
#include <tchar.h>
#include "../../Library/RainlendarAPI.h"
#include "resource.h"
#include "OutlookPlugin.h"
#include <commctrl.h>

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

extern HINSTANCE g_Instance;
extern std::tstring g_OutlookProfile;
extern bool g_KeepAlive;
extern bool g_UseLabels;
extern bool g_NoMessageBody;
extern bool g_NoDoneTasks;
extern std::vector<MessageStoreName> g_Stores;
std::vector<MessageStoreName> g_AllStores;

void GetMessageStores(std::vector<MessageStoreName>& stores);

#define PLUGIN_NAME _T("OutlookPlugin")

BOOL OnInitSetupDialog(HWND hwndDlg)
{
	SetWindowText(hwndDlg, Plugin_GetName());
	
	HWND widget = GetDlgItem(hwndDlg, IDOK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 0, _T("OK")));

	widget = GetDlgItem(hwndDlg, IDCANCEL);
	if (widget) SetWindowText(widget, Rainlendar_GetString(_T("General"), 1, _T("Cancel")));

	widget = GetDlgItem(hwndDlg, IDC_OUTLOOK_PROFILE_STATIC);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 0, _T("Outlook profile:")));

	widget = GetDlgItem(hwndDlg, IDC_LABELS_CHECK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 1, _T("Use Outlook labels as event profiles")));

	widget = GetDlgItem(hwndDlg, IDC_KEEPALIVE_CHECK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 2, _T("Keep MAPI-connection alive")));

	widget = GetDlgItem(hwndDlg, IDC_NO_MESSAGE_CHECK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 3, _T("Don't show message body")));

	widget = GetDlgItem(hwndDlg, IDC_NO_DONE_TASKS_CHECK);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 4, _T("Don't show completed tasks")));

	widget = GetDlgItem(hwndDlg, IDC_GENERAL_GROUP);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 5, _T("General Settings")));

	widget = GetDlgItem(hwndDlg, IDC_MESSAGE_STORES_GROUP);
	if (widget) SetWindowText(widget, Rainlendar_GetString(PLUGIN_NAME, 6, _T("Imported message stores")));

	widget = GetDlgItem(hwndDlg, IDC_PROFILE_EDIT);
	if (widget) SetWindowText(widget, g_OutlookProfile.c_str());

	if (g_UseLabels)
	{
		CheckDlgButton(hwndDlg, IDC_LABELS_CHECK, BST_CHECKED);
	}

	if (g_KeepAlive)
	{
		CheckDlgButton(hwndDlg, IDC_KEEPALIVE_CHECK, BST_CHECKED);
	}

	if (g_NoMessageBody)
	{
		CheckDlgButton(hwndDlg, IDC_NO_MESSAGE_CHECK, BST_CHECKED);
	}

	if (g_NoDoneTasks)
	{
		CheckDlgButton(hwndDlg, IDC_NO_DONE_TASKS_CHECK, BST_CHECKED);
	}

	widget = GetDlgItem(hwndDlg, IDC_STORE_LIST);
	if(widget) 
	{
		g_AllStores.clear();
		GetMessageStores(g_AllStores);

		// Add Columns
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH; 
		lvc.cx = 250;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(widget, 0, &lvc);
		
		ListView_SetExtendedListViewStyle(widget, LVS_EX_CHECKBOXES);

		int count = 0;
		for (int i = 0; i < g_AllStores.size(); i++)
		{
			for (int j = 0; j < g_AllStores[i].folders.size(); j++)
			{
				std::string name;

				name = g_AllStores[i].name;
				name += " - ";
				name += g_AllStores[i].folders[j];

				LVITEM lvI; 
				
				lvI.mask = LVIF_TEXT;
				lvI.iItem = count;
				lvI.iSubItem = 0;
				lvI.pszText = (char*)name.c_str();
				lvI.iItem = count;
				ListView_InsertItem(widget, &lvI);

				for (int k = 0; k < g_Stores.size(); k++)
				{
					if (g_AllStores[i].name.empty()) 
					{
						// Compare with names
						if (g_Stores[k].name == g_AllStores[i].name)
						{
							for (int l = 0; l < g_Stores[k].folders.size(); l++)
							{
								if (g_Stores[k].folders[l] == g_AllStores[i].folders[j])
								{
									ListView_SetCheckState(widget, count, TRUE);
								}
							}
						}
					}
					else
					{
						// Compare with ids
						if (g_Stores[k].storeID == g_AllStores[i].storeID)
						{
							for (int l = 0; l < g_Stores[k].folderIDs.size(); l++)
							{
								if (g_Stores[k].folderIDs[l] == g_AllStores[i].folderIDs[j])
								{
									ListView_SetCheckState(widget, count, TRUE);
								}
							}
						}
					}
				}
				count++;
			}
		}
	}

	return TRUE;
}

void UpdateSettings(HWND hwndDlg)
{
	TCHAR buffer[1024];
	HWND widget = GetDlgItem(hwndDlg, IDC_PROFILE_EDIT);
	GetWindowText(widget, buffer, 1024);
	g_OutlookProfile = buffer;

	g_UseLabels = (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_LABELS_CHECK));
	g_KeepAlive = (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_KEEPALIVE_CHECK));
	g_NoMessageBody = (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_NO_MESSAGE_CHECK));
	g_NoDoneTasks = (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_NO_DONE_TASKS_CHECK));

	g_Stores.clear();

	widget = GetDlgItem(hwndDlg, IDC_STORE_LIST);

	int count = 0;
	for (int i = 0; i < g_AllStores.size(); i++)
	{
		MessageStoreName store;
		store.name = g_AllStores[i].name;
		store.storeID = g_AllStores[i].storeID;

		for (int j = 0; j < g_AllStores[i].folders.size(); j++)
		{
			if (ListView_GetCheckState(widget, count++))
			{
				store.folders.push_back(g_AllStores[i].folders[j]);
				store.folderIDs.push_back(g_AllStores[i].folderIDs[j]);
			}
		}

		if (g_AllStores[i].folders.size() > 0)
		{
			g_Stores.push_back(store);
		}
	}
	
//	widget = GetDlgItem(hwndDlg, IDC_STORE_TREE);
//	HTREEITEM parent = NULL;
//	HTREEITEM child = NULL;
//	parent = TreeView_GetRoot(widget);
//	while (parent != NULL)
//	{
//		MessageStoreName store;
//
//		TVITEM tvitem;
//		tvitem.mask = TVIF_PARAM | TVIF_HANDLE | TVIF_TEXT;
//		tvitem.hItem = parent;
//		tvitem.pszText = buffer;
//		tvitem.cchTextMax = 1024;
//		TreeView_GetItem(widget, &tvitem);
//
//		int i = tvitem.lParam;
//		store.name = buffer;
//
//		child = TreeView_GetChild(widget, parent);
//		while (child != NULL)
//		{
//			if (TreeView_GetCheckState(widget, child) == 1)
//			{
//				TVITEM tvitem;
//				tvitem.mask = TVIF_PARAM | TVIF_HANDLE | TVIF_TEXT;
//				tvitem.hItem = child;
//				tvitem.pszText = buffer;
//				tvitem.cchTextMax = 1024;
//				TreeView_GetItem(widget, &tvitem);
//
//				int j = tvitem.lParam;
//				store.folders.push_back(buffer);
//			}
//			child = TreeView_GetNextSibling(widget, child);
//		}			
//
//		parent = TreeView_GetNextSibling(widget, parent);
//
//		g_Stores.push_back(store);
//	}
}

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitSetupDialog(hwndDlg);
	
        case WM_CLOSE:
			EndDialog(hwndDlg, IDCANCEL);
            return TRUE; 
			
        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDOK:
					UpdateSettings(hwndDlg);
					EndDialog(hwndDlg, IDOK);
                    return TRUE; 

                case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 

