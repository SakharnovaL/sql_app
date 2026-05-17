#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Del_BD : public wxDialog{
public:
    Del_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int id);
private:
    sqlite3* m_bd;
    wxString m_table_name;
    int m_id;
    bool m_deleted;

    void OnDelete(wxCommandEvent& event);
};