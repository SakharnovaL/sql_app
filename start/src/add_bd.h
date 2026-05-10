#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Add_New_BD : public wxDialog{
public:
    Add_New_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name);
private:
    sqlite3* m_bd;
    const wxString m_table_name;
    int m_id;
    int m_col;
    wxTextCtrl* m_name;
    wxListCtrl* m_list_add;
    std::vector<wxString> m_cur_row;        //значение полей
    std::vector<wxString> m_cur_col;        //имена полей
    bool m_saved;

    void OnOk(wxCommandEvent& event);
    void LoadTableStruct();
    void OnItemActivated(wxListEvent& event);
    void AddRecord();
};