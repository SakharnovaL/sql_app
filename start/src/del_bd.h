#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "base_class.h"

class Del_BD : public wxDialog, public Base_Class{
public:
    Del_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int id);
private:
    int m_id;
    bool m_deleted;

    void OnDelete(wxCommandEvent& event);
};