#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "del_bd.h"


Del_BD::Del_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("подтверждение удаления"), wxDefaultPosition, wxSize(400, 200)){
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* war_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* quest_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* war_label = new wxStaticText(panel, wxID_ANY, wxT("ВНИМАНИЕ!!!"));
    war_sizer->Add(war_label);

    wxStaticText* quest_label = new wxStaticText(panel, wxID_ANY, wxT("Вы действительно хотите удалить запись"));       //надо добавить название записи
    quest_sizer->Add(quest_label);

    wxStaticText* info_label = new wxStaticText(panel, wxID_ANY, wxT("это действие нельзя отменить"));
    info_sizer->Add(info_label);
    
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("да, удалить"));
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn);
    btn_sizer->Add(cancel_btn);

    main_sizer->Add(war_sizer);
    main_sizer->Add(quest_sizer);
    main_sizer->Add(info_sizer);
    main_sizer->Add(btn_sizer);
    
    panel->SetSizer(main_sizer);
};