#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "del_bd.h"
#include <wx/statline.h>


Del_BD::Del_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int id) : wxDialog(parent, wxID_ANY, wxT("подтверждение удаления"), wxDefaultPosition, wxSize(400, 200)), m_bd(bd), m_table_name(table_name), m_id(id), m_deleted(false){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(255, 255, 255));
    
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* war_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* warning_icon = new wxStaticText(panel, wxID_ANY, wxT("⚠️"));
    wxFont iconFont = warning_icon->GetFont();
    iconFont.SetPointSize(24);
    warning_icon->SetFont(iconFont);
    warning_icon->SetForegroundColour(wxColour(255, 0, 0));
    
    wxStaticText* war_label = new wxStaticText(panel, wxID_ANY, wxT("ВНИМАНИЕ!"));
    wxFont warFont = war_label->GetFont();
    warFont.SetWeight(wxFONTWEIGHT_BOLD);
    warFont.SetPointSize(14);
    war_label->SetFont(warFont);
    war_label->SetForegroundColour(wxColour(255, 0, 0));
    
    war_sizer->AddStretchSpacer();
    war_sizer->Add(warning_icon, 0, wxRIGHT, 10);
    war_sizer->Add(war_label, 0);
    war_sizer->AddStretchSpacer();
    main_sizer->Add(war_sizer, 0, wxTOP | wxEXPAND, 20);

    main_sizer->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 20);

    wxBoxSizer* quest_sizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* quest_label = new wxStaticText(panel, wxID_ANY, wxString::Format(wxT("Вы действительно хотите удалить запись?")));
    wxFont questFont = quest_label->GetFont();
    questFont.SetWeight(wxFONTWEIGHT_BOLD);
    quest_label->SetFont(questFont);
    quest_sizer->Add(quest_label, 0, wxALIGN_CENTER | wxBOTTOM, 15);

    wxStaticBox* info_box = new wxStaticBox(panel, wxID_ANY, wxT("Информация о записи"));
    wxStaticBoxSizer* info_box_sizer = new wxStaticBoxSizer(info_box, wxVERTICAL);
    
    wxString info_text = wxString::Format(wxT("ID записи: %d"), m_id);
    wxStaticText* info_label = new wxStaticText(panel, wxID_ANY, info_text);
    info_label->SetForegroundColour(wxColour(80, 80, 80));
    info_box_sizer->Add(info_label, 0, wxALL | wxALIGN_CENTER, 10);
    
    quest_sizer->Add(info_box_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);
    main_sizer->Add(quest_sizer, 0, wxALL | wxEXPAND, 15);

    wxBoxSizer* warning_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* warn_icon = new wxStaticText(panel, wxID_ANY, wxT("⚠️"));
    warn_icon->SetForegroundColour(wxColour(255, 140, 0));
    
    wxStaticText* irreversible_label = new wxStaticText(panel, wxID_ANY, wxT("Это действие НЕЛЬЗЯ отменить!"));
    irreversible_label->SetForegroundColour(wxColour(255, 140, 0));
    irreversible_label->SetFont(irreversible_label->GetFont().Bold());
    
    warning_sizer->AddStretchSpacer();
    warning_sizer->Add(warn_icon, 0, wxRIGHT, 5);
    warning_sizer->Add(irreversible_label, 0);
    warning_sizer->AddStretchSpacer();
    main_sizer->Add(warning_sizer, 0, wxALL | wxEXPAND, 10);

    main_sizer->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 20);
    
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("Да, удалить"));
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("Отмена"));
    
    ok_btn->SetBackgroundColour(wxColour(220, 80, 80));
    ok_btn->SetForegroundColour(wxColour(255, 255, 255));
    
    ok_btn->Bind(wxEVT_BUTTON, &Del_BD::OnDelete, this);
    
    btn_sizer->AddStretchSpacer();
    btn_sizer->Add(ok_btn, 0, wxRIGHT, 15);
    btn_sizer->Add(cancel_btn, 0);
    btn_sizer->AddStretchSpacer();
    main_sizer->Add(btn_sizer, 0, wxALL | wxEXPAND, 20);
    
    panel->SetSizer(main_sizer);

    Layout();
    Fit();
};

void Del_BD::OnDelete(wxCommandEvent& event){
    wxString maxIdSql = wxString::Format("SELECT MAX(id) FROM %s", m_table_name);
    int maxId = 0;
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(m_bd, maxIdSql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            maxId = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    // Удаляем запись
    wxString sql = wxString::Format("DELETE FROM %s WHERE id = %d", m_table_name, m_id);
    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_bd, sql.ToUTF8(), nullptr, nullptr, &err_msg);
    
    if(rc != SQLITE_OK){
        wxMessageBox(wxString::Format(wxT("Ошибка удаления: %s"), wxString::FromUTF8(err_msg)));
        sqlite3_free(err_msg);
        EndModal(wxID_CANCEL);
        return;
    }
    
    // Если удалили последнюю запись, сбрасываем счетчик
    if(m_id == maxId && maxId > 0){
        wxString resetSql = wxString::Format("UPDATE sqlite_sequence SET seq = %d WHERE name = '%s'", maxId - 1, m_table_name);
        sqlite3_exec(m_bd, resetSql.ToUTF8(), nullptr, nullptr, nullptr);
    }

    EndModal(wxID_OK);
}