#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class MyApp : public wxApp{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class Dialog_Create_BD : public wxDialog{
public:
    Dialog_Create_BD(wxWindow* parent);         //создание самого диалогового окна
    wxTextCtrl* m_nameBD;                       //поле для ввода названия
    wxTextCtrl* m_pathBD;                       //поле для ввода пути к бд

    void OnOk(wxCommandEvent& event);           //отработка кнопки создать
    //void OnBack(wxCommandEvent& event);         //отработка кнопки отмена
    void OnBrowse(wxCommandEvent& evenr);       //отработка кнопки обзор для выбора пути 
};

class Start_Frame : public wxFrame{
public:
    wxTextCtrl* m_findBD;

    Start_Frame(wxWindow* parent, wxString title = wxT("Менеджер баз данных"));
    void OnNewBD(wxCommandEvent& event);
    void OnOpenBD(wxCommandEvent& event);
    void OnAddNewBD(wxCommandEvent& event);
    void OnEditBD(wxCommandEvent& event);
    void OnDelBd(wxCommandEvent& event);
private:
    sqlite3* m_bd;
    wxListCtrl* m_list;
    wxChoice* m_table_choice;

    void LoadTables();
    void LoadTableData(const wxString& table_name);
    void OnTableSelected(wxCommandEvent &event);
    void OpenBD(const wxString& dbPath);
};