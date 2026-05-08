#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

class Open_BD : public wxDialog{
public:
    Open_BD(wxWindow* parent);
    wxString GetSelPath() const;
    void OnOk(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);
private:
    wxTextCtrl* m_chooseBD;                     //строчка с выбранным бд
    wxStaticText* m_info;                       //табличка с инфой о файле
    wxString m_path;                            //путь к бд
};