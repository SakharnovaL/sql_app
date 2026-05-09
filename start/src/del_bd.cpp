#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "del_bd.h"


Del_BD::Del_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int id) : wxDialog(parent, wxID_ANY, wxT("подтверждение удаления"), wxDefaultPosition, wxSize(400, 200)), m_bd(bd), m_table_name(table_name), m_id(id), m_deleted(false){
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* war_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* quest_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* war_label = new wxStaticText(panel, wxID_ANY, wxT("ВНИМАНИЕ!!!"));
    war_sizer->Add(war_label, 0, wxALIGN_CENTER);

    wxStaticText* quest_label = new wxStaticText(panel, wxID_ANY, wxString::Format(wxT("Вы действительно хотите удалить запись?\n\n" "Таблица: %s\n" "ID записи: %d\n"), m_table_name, m_id));       //надо добавить название записи
    quest_sizer->Add(quest_label, 0, wxALL | wxALIGN_CENTER, 10);

    wxStaticText* info_label = new wxStaticText(panel, wxID_ANY, wxT("это действие нельзя отменить"));
    info_sizer->Add(info_label, 0, wxALIGN_CENTER);
    
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("да, удалить"));
    ok_btn->Bind(wxEVT_BUTTON, &Del_BD::OnDelete, this);
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->AddStretchSpacer();
    btn_sizer->Add(ok_btn, 0, wxRIGHT, 10);
    btn_sizer->Add(cancel_btn, 0);
    btn_sizer->AddStretchSpacer();

    main_sizer->Add(war_sizer, 0, wxALL | wxEXPAND, 10);
    main_sizer->Add(quest_sizer, 0, wxALL | wxEXPAND, 10);
    main_sizer->Add(info_sizer, 0, wxALL | wxEXPAND, 10);
    main_sizer->Add(btn_sizer, 0, wxALL | wxEXPAND, 10);
    
    panel->SetSizer(main_sizer);
    Layout();
    Fit();
};

void Del_BD::OnDelete(wxCommandEvent& event){
    wxString sql = wxString::Format("DELETE FROM %s WHERE rowid = %d;", m_table_name, m_id);

    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_bd, sql.ToUTF8(), nullptr, nullptr, &err_msg);
    if(rc != SQLITE_OK){
        wxMessageBox(wxString::Format(wxT("Ошибка удаления:\n%s"), wxString::FromUTF8(err_msg)), wxT("Ошибка"), wxOK | wxICON_ERROR);
        sqlite3_free(err_msg);
        m_deleted = false;
        EndModal(wxID_CANCEL);
    }
    else{
        m_deleted = true;
        EndModal(wxID_OK);
    }
}