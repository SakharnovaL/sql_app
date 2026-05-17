#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include <vector>
#include "base_class.h"

class Edit_BD : public wxDialog, public Base_Class{
public:
    Edit_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int record_id);
private:
    int m_id;
    int m_col;
    wxTextCtrl* m_name;
    wxListCtrl* m_list_edit;
    std::vector<wxString> m_cur_row;        //значение полей
    std::vector<wxString> m_cur_col;        //имена полей
    
    void OnOk(wxCommandEvent& event);
    void LoadTableStruct();
    void LoadRecord(int id);
    void SaveRecord();
    void OnItemActivated(wxListEvent& event);
    void OnSaveClic(wxCommandEvent& event){SaveRecord();};
};