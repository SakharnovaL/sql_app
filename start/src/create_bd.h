#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Create_BD : public wxDialog{
public:
    Create_BD(wxWindow* parent);         //создание самого диалогового окна
    wxTextCtrl* m_nameBD;                       //поле для ввода названия
    wxTextCtrl* m_pathBD;                       //поле для ввода пути к бд

    void OnOk(wxCommandEvent& event);           //отработка кнопки создать
    //void OnBack(wxCommandEvent& event);         //отработка кнопки отмена
    void OnBrowse(wxCommandEvent& evenr);       //отработка кнопки обзор для выбора пути 
};