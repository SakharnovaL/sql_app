#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "base_class.h"

class Add_New_BD : public wxDialog, public Base_Class{
public:
    Add_New_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name);
private:
    int m_id;
    int m_col;
    wxTextCtrl* m_name;
    wxListCtrl* m_list_add;
    std::vector<wxString> m_cur_row;        //значение полей
    std::vector<wxString> m_cur_col;        //имена полей
    int m_nextId;

    void OnOk(wxCommandEvent& event);
    void LoadTableStruct();
    void OnItemActivated(wxListEvent& event);
    void AddRecord();
    int GetNextId();
};