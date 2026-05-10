#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "create_bd.h"

Create_BD::Create_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Создание новой бд"), wxDefaultPosition, wxSize(400, 200)){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);                                            //создание общего бокса куда запихаем все состовляющие окна
    wxBoxSizer* name_sizer = new wxBoxSizer(wxHORIZONTAL);                                          //бокс для отдела связанного с заведением названия бд
    wxBoxSizer* path_sizer = new wxBoxSizer(wxHORIZONTAL);                                          //бокс для отдела связанного с выбором пути, где валяется бд
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);                                           //бокс для кнопочек

    wxStaticText* name_label = new wxStaticText(panel, wxID_ANY, wxT("введите название бд:"));      //статичный текст 
    m_nameBD = new wxTextCtrl(panel, wxID_ANY, wxT("новая база"));                                  //поле куда можно ввести название бд, по умолчанию - новая база
    name_sizer->Add(name_label);
    name_sizer->Add(m_nameBD);

    wxStaticText* path_lable = new wxStaticText(panel, wxID_ANY, wxT("выберете путь:"));            //статичный текст
    m_pathBD = new wxTextCtrl(panel, wxID_ANY, wxGetCwd());                                         //поле куда можно вписать путь бд, wxGetCwd() возвращает текущий рабочий каталог - значение по умолчанию
    wxButton* browse_btn = new wxButton(panel, wxID_ANY, wxT("обзор"));
    browse_btn->Bind(wxEVT_BUTTON, &Create_BD::OnBrowse, this);                              //подключение кнопки, она очень волшебная, я хуй знает как она работает
    path_sizer->Add(path_lable);
    path_sizer->Add(m_pathBD);
    path_sizer->Add(browse_btn);

    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("создать"));
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn);
    btn_sizer->Add(cancel_btn);

    main_sizer->Add(name_sizer);
    main_sizer->Add(path_sizer);
    main_sizer->Add(btn_sizer);

    panel->SetSizer(main_sizer);
};

void Create_BD::OnBrowse(wxCommandEvent& event){                                             //Открывает диалог выбора папки (wxDirDialog) Начальный путь — текущее значение из поля пути Если пользователь выбрал папку (нажал OK), обновляет поле пути
    wxDirDialog dlg(this, "Выберите папку для сохранения базы данных", m_pathBD->GetValue());
    if (dlg.ShowModal() == wxID_OK){
        m_pathBD->SetValue(dlg.GetPath());
    }
};

void Create_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    wxString dbName = m_nameBD->GetValue();   // читаем название
    wxString dbPath = m_pathBD->GetValue();   // читаем путь
    
    if(dbName.IsEmpty()){
        wxMessageBox("Введите название базы данных!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    
    wxMessageBox(wxString::Format("База данных '%s' создана в папке '%s'", dbName, dbPath), "Успех", wxOK);
    
    EndModal(wxID_OK);
};