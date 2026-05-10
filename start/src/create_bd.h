#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Create_BD : public wxDialog{
public:
    Create_BD(wxWindow* parent);         //создание самого диалогового окна
private:
    sqlite3* m_bd;
    wxListCtrl* m_list_create;
    wxTextCtrl* m_nameBD;                       //поле для ввода названия
    wxTextCtrl* m_pathBD;                       //поле для ввода пути к бд
    wxStaticText* m_info;
    wxString m_bdpath;
    std::vector<wxString> m_col_name;

    void OnOk(wxCommandEvent& event);           //отработка кнопки создать
    void OnBrowse(wxCommandEvent& evenr);       //отработка кнопки обзор для выбора пути
    void OnAddCol(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnCellEdit(wxListEvent& event);
    void OnColEdit(wxListEvent& event);
    std::vector<wxString> GetColumnNAme();
    std::vector<std::vector<wxString>> GetTableData();
    bool CreateBD();
    void OnItemActivated(wxListEvent& event);
};