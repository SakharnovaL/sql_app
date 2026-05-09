#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Add_new_BD : public wxDialog{
public:
    Add_new_BD(wxWindow* parent);
    wxTextCtrl* m_name;
    wxTextCtrl* m_meaning;
    wxTextCtrl* m_discribe;

    void OnOk(wxCommandEvent& event);
};