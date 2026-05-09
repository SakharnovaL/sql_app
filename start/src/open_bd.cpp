#include <wx/wx.h>
#include "sqlite3.h"
#include <iostream>
#include "open_bd.h"
#include <wx/filename.h>

Open_BD::Open_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("открыть базу данных"), wxDefaultPosition, wxSize(500, 400)){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* text_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* text_label = new wxStaticText(panel, wxID_ANY, wxT("выберете базу данных для открытия"));
    text_sizer->AddStretchSpacer();
    text_sizer->Add(text_label, 0, wxALIGN_CENTER);                                                                                //надо сделать посередине окна
    text_sizer->AddStretchSpacer();
    main_sizer->Add(text_sizer, 0, wxALL | wxEXPAND, 10);

    wxBoxSizer* last_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* last_label = new wxStaticText(panel, wxID_ANY, wxT("последние баззы данных"));                //надо добавить окно с последними выбранными бд
    last_sizer->Add(last_label, 0, wxLEFT | wxBOTTOM, 5);
    main_sizer->Add(last_sizer, 0, wxALL | wxEXPAND, 10);

    wxBoxSizer* choose_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* choose_label = new wxStaticText(panel, wxID_ANY, wxT("или выберете файл"));
    m_chooseBD = new wxTextCtrl(panel, wxID_ANY, wxGetCwd());
    wxButton* browse_btn = new wxButton(panel, wxID_ANY, wxT("обзор"));
    browse_btn->Bind(wxEVT_BUTTON, &Open_BD::OnBrowse, this);
    choose_sizer->Add(choose_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    choose_sizer->Add(m_chooseBD, 1, wxRIGHT | wxEXPAND, 5);
    choose_sizer->Add(browse_btn, 0);
    main_sizer->Add(choose_sizer, 0, wxALL | wxEXPAND, 10);
    
    m_info = new wxStaticText(panel, wxID_ANY, wxT("информация о бд появится здесь"));
    wxStaticBox* info_box = new wxStaticBox(panel, wxID_ANY, wxT("информация о бд"));
    wxStaticBoxSizer* info_sizer = new wxStaticBoxSizer(info_box, wxVERTICAL);
    info_sizer->Add(m_info, 1, wxLEFT | wxEXPAND, 10);
    main_sizer->Add(info_sizer, 1, wxALL | wxEXPAND, 10);

    main_sizer->AddStretchSpacer();

    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("открыть"));
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));

    ok_btn->Bind(wxEVT_BUTTON, &Open_BD::OnOk, this);

    btn_sizer->AddStretchSpacer();
    btn_sizer->Add(ok_btn, 0, wxRight, 5);
    btn_sizer->Add(cancel_btn, 0);
    main_sizer->Add(btn_sizer, 0, wxALL | wxEXPAND, 10);

    panel->SetSizer(main_sizer);
};

void Open_BD::OnBrowse(wxCommandEvent& event){                                             //Открывает диалог выбора папки (wxDirDialog) Начальный путь — текущее значение из поля пути Если пользователь выбрал папку (нажал OK), обновляет поле пути
    wxFileDialog dlg(this, wxT("Выберите папку для сохранения базы данных"), m_chooseBD->GetValue(), "", "SQLite DB files (*.db;*.sqlite;*.sqlite3)|*.db;*.sqlite;*.sqlite3|All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);         //выбирает только файлы с расширенем бд
    if (dlg.ShowModal() == wxID_OK){
        wxString file = dlg.GetPath();          //записывает полный путь
        m_chooseBD->SetValue(file);             //вставляем в текстовое поле путь

        wxFileName fileInfo(file);
        if(fileInfo.FileExists()){              //если не было инфы о бд, то заводим ее
            wxString info;
            info += wxT("имя: ") + fileInfo.GetFullName() + "\n";
            info += wxT("путь: ") + fileInfo.GetFullPath() + "\n";
            info += wxT("размер: ") + wxString::Format("%.2f KB", fileInfo.GetSize().GetValue() / 1024.0) + "\n";
            info += wxT("создана: ") + fileInfo.GetModificationTime().Format("%d-%m-%Y %H:%M:%S") + "\n";

            sqlite3* bd = nullptr;
            if(sqlite3_open(file.ToUTF8(), &bd) == SQLITE_OK){      //открываем бд
                sqlite3_stmt* stmt;
                int table_cnt = 0;
                if(sqlite3_prepare_v2(bd, "SELECT COUNT(*) FROM sqlite_master WHERE type='table';", -1, &stmt, nullptr) == SQLITE_OK){      //подсчитывает кол-во таблиц в бд
                    if(sqlite3_step(stmt) == SQLITE_ROW){           //выполняет запрос и если строка не пустая, то переходит к первой строке результата
                        table_cnt = sqlite3_column_int(stmt, 0);    //берет значение из первого столбца
                    }
                    sqlite3_finalize(stmt);                         //очищает подготовленный запрос, чтоб программа не умерла
                }
                sqlite3_close(bd);
                info += wxT("таблиц: ") + wxString::Format("%d", table_cnt);
            }
            m_info->SetLabel(info);
        }
    }
};

void Open_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    if(m_chooseBD == nullptr){
        wxMessageBox(wxT("m_chooseBD is NULL!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        event.Skip(false);
        return;
    }
    wxString dbPath = m_chooseBD->GetValue();   // читаем путь
    
    if(dbPath.IsEmpty()){
        wxMessageBox(wxT("Введите путь базы данных!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        event.Skip(false);
        return;
    }
    
    m_path = dbPath;
    
    event.Skip(true);
};

wxString Open_BD::GetSelPath() const{
    return m_path;
}