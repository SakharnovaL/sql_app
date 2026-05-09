#include <wx/wx.h>
#include "sqlite3.h"
#include <iostream>
#include "edit_bd.h"

Edit_BD::Edit_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("редактирование записи"), wxDefaultPosition, wxSize(400, 200)){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* text_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* name_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* type_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* meaning_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* discribe_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* text_label = new wxStaticText(panel, wxID_ANY, wxT("редактирование записи"));
    text_sizer->Add(text_label);

    wxStaticText* id_label = new wxStaticText(panel, wxID_ANY, wxT("ID записи:"));
    wxStaticText* crete_label = new wxStaticText(panel, wxID_ANY, wxT("дата создания:"));
    wxStaticText* change_label = new wxStaticText(panel, wxID_ANY, wxT("дата изменения:"));
    info_sizer->Add(id_label);
    info_sizer->Add(crete_label);
    info_sizer->Add(change_label);

    wxStaticText* name_lable = new wxStaticText(panel, wxID_ANY, wxT("название поля:"));
    m_name = new wxTextCtrl(panel, wxID_ANY, wxT("новое поле"));
    name_sizer->Add(name_lable);
    name_sizer->Add(m_name);

    wxStaticText* type_label = new wxStaticText(panel, wxID_ANY, wxT("тип данных:"));
    wxRadioButton* btn_text = new wxRadioButton(panel, wxID_ANY, wxT("текст"));
    wxRadioButton* btn_number = new wxRadioButton(panel, wxID_ANY, wxT("число"));
    wxRadioButton* btn_data = new wxRadioButton(panel, wxID_ANY, wxT("дата"));
    wxRadioButton* btn_log = new wxRadioButton(panel, wxID_ANY, wxT("логический"));
    type_sizer->Add(type_label);
    type_sizer->Add(btn_text);
    type_sizer->Add(btn_number);
    type_sizer->Add(btn_data);
    type_sizer->Add(btn_log);

    wxStaticText* mean_label = new wxStaticText(panel, wxID_ANY, wxT("значение:"));
    m_meaning = new wxTextCtrl(panel, wxID_ANY);
    meaning_sizer->Add(mean_label);
    meaning_sizer->Add(m_meaning);

    wxStaticText* disc_label = new wxStaticText(panel, wxID_ANY, wxT("описание (необязвтельно):"));
    m_discribe = new wxTextCtrl(panel, wxID_ANY);
    discribe_sizer->Add(disc_label);
    discribe_sizer->Add(m_discribe);

    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("сохранить"));
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn);
    btn_sizer->Add(cancel_btn);

    main_sizer->Add(text_sizer);
    main_sizer->Add(info_sizer);
    main_sizer->Add(name_sizer);
    main_sizer->Add(type_sizer);
    main_sizer->Add(meaning_sizer);
    main_sizer->Add(discribe_sizer);
    main_sizer->Add(btn_sizer);

    panel->SetSizer(main_sizer);
};

void Edit_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    wxString dbName = m_name->GetValue();   // читаем путь
    wxString dbMeaning = m_meaning->GetValue();
    wxString dbDiscribe = m_discribe->GetValue();
    
    if(dbName.IsEmpty() || dbMeaning.IsEmpty() || dbDiscribe.IsEmpty()){
        wxMessageBox("Введите название, значение и описание поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbName.IsEmpty() || dbMeaning.IsEmpty()){
        wxMessageBox("Введите название, значение поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbName.IsEmpty() || dbDiscribe.IsEmpty()){
        wxMessageBox("Введите название, описание поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbDiscribe.IsEmpty() || dbMeaning.IsEmpty()){
        wxMessageBox("Введите значение и его описание поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbName.IsEmpty()){
        wxMessageBox("Введите название поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbMeaning.IsEmpty()){
        wxMessageBox("Введите значение поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    else if(dbDiscribe.IsEmpty()){
        wxMessageBox("Введите описание поля", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    
    wxMessageBox(wxString::Format("Создано поле с названием '%s', значением '%s', описанием '%s'", dbName, dbMeaning, dbDiscribe), "Успех", wxOK);
    
    EndModal(wxID_OK);
};