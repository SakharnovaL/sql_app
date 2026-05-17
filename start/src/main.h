#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "base_class.h"

class MyApp : public wxApp{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class Start_Frame : public wxFrame, public Base_Class{
public:
    wxTextCtrl* m_findBD;

    Start_Frame(wxWindow* parent, wxString title = wxT("Менеджер баз данных"));
    void OnNewBD(wxCommandEvent& event);
    void OnOpenBD(wxCommandEvent& event);
    void OnAddNewBD(wxCommandEvent& event);
    void OnEditBD(wxCommandEvent& event);
    void OnDelBd(wxCommandEvent& event);
private:
    wxListCtrl* m_list;
    wxChoice* m_table_choice;
    wxStaticText* m_dbNameLabel;

    void LoadTables();
    void LoadTableData(const wxString& table_name);
    void OnTableSelected(wxCommandEvent &event);
    void OpenBD(const wxString& dbPath);
};