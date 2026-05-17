#include <wx/wx.h>
#include "sqlite3.h"
#include <iostream>
#include "edit_bd.h"


Edit_BD::Edit_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name, int record_id) : wxDialog(parent, wxID_ANY, wxT("редактирование записи"), wxDefaultPosition, wxSize(400, 300)), m_bd(bd), m_table_name(table_name), m_id(record_id), m_col(){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    wxBoxSizer* text_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* text_label = new wxStaticText(panel, wxID_ANY, wxT("редактирование записи"));
    text_sizer->Add(text_label);
    main_sizer->Add(text_sizer, 0, wxALL | wxALIGN_CENTER, 10);

    m_list_edit = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
    m_list_edit->InsertColumn(0, wxT("поле"));
    m_list_edit->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    m_list_edit->InsertColumn(1, wxT("значение"));
    m_list_edit->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    main_sizer->Add(m_list_edit, 1, wxALL | wxEXPAND, 10);

    LoadTableStruct();

    m_list_edit->Bind(wxEVT_LIST_ITEM_ACTIVATED, &Edit_BD::OnItemActivated, this);

    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("сохранить"));
    ok_btn->Bind(wxEVT_BUTTON, &Edit_BD::OnOk, this);
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn, 0, wxALL, 5);
    btn_sizer->Add(cancel_btn, 0, wxALL, 5);
    main_sizer->Add(btn_sizer, 0, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(main_sizer);

    LoadRecord(m_id);
};

void Edit_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    SaveRecord();
    EndModal(wxID_OK);
};

void Edit_BD::LoadTableStruct(){
    wxString col_tab = wxString::Format("PRAGMA table_info(%s);", m_table_name);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(m_bd, col_tab.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW){
            const char* col_name = (const char*)sqlite3_column_text(stmt, 1);
            if(col_name){
                wxString col = wxString::FromUTF8(col_name);
                m_cur_col.push_back(col);
            }
        }
        sqlite3_finalize(stmt);
    }

    wxString countSql = wxString::Format("SELECT COUNT(*) FROM %s;", m_table_name);
    if(sqlite3_prepare_v2(m_bd, countSql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            m_col = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
}

void Edit_BD::LoadRecord(int id){
    if(m_bd == nullptr){
        wxMessageBox(wxT("LoadTables: m_bd == nullptr"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    if(id == -1){
        wxString count_sql = wxString::Format("SELECT MIN(id) FROM %s", m_table_name);
        sqlite3_stmt* stmt;                                                                     //указатель на заготовленный sql запрос, сюда записывается результат sql запроса
        if(sqlite3_prepare_v2(m_bd, count_sql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
            if(sqlite3_step(stmt) == SQLITE_ROW){
                id = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }

        if(id == -1){
            m_list_edit->ClearAll();
            m_list_edit->InsertItem(0, wxT("нет записей для редактирования"));
            m_id = -1;
            return;
        }
    }

    m_id = id;
    m_list_edit->DeleteAllItems();
    m_cur_row.clear();

    wxString sql = wxString::Format("SELECT * FROM %s WHERE id = %d;", m_table_name, id);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(m_bd, sql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            int col_cnt = sqlite3_column_count(stmt);

            for(int i = 0; i < col_cnt && i < (int)m_cur_col.size(); i++){
                const char* val = (const char*)sqlite3_column_text(stmt, i);
                wxString cell_val;
                if(val != nullptr){
                    cell_val = wxString::FromUTF8(val);
                }
                else{
                    cell_val = "";
                }
                m_cur_row.push_back(cell_val);
                
                long item_index = m_list_edit->GetItemCount();
                m_list_edit->InsertItem(item_index, m_cur_col[i]);
                m_list_edit->SetItem(item_index, 1, cell_val);                                   //1 так как значение колонки куда мы пишем всегда 1
                
                wxString fieldName = m_cur_col[i];
                if(fieldName == "id" || fieldName == "ID"){
                    m_list_edit->SetItemBackgroundColour(item_index, wxColour(230, 230, 230));
                }
            }
        }
        else{
            wxMessageBox(wxString::Format(wxT("Запись с ID %d не найдена"), id), wxT("Ошибка"), wxOK | wxICON_ERROR);
        }
        sqlite3_finalize(stmt);
    }
}

void Edit_BD::SaveRecord(){
    if(m_id == -1){
        return;
    }

    wxString set_change;
    for(size_t i = 0; i < m_cur_col.size(); i ++){
        wxString col_name = m_cur_col[i];
        if(col_name == "id" || col_name == "ID"){
            continue;  // пропускаем ID
        }

        if(i < m_cur_row.size()){
            wxString val = m_cur_row[i];
            val.Replace("'", "''");
            if(set_change.IsEmpty() == false){
                set_change += ", ";
            }
            set_change += wxString::Format("%s = '%s'", m_cur_col[i], val);
        }
    }

    wxString sql = wxString::Format("UPDATE %s SET %s WHERE id = %d;", m_table_name, set_change, m_id);
    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_bd, sql.ToUTF8(), nullptr, nullptr, &err_msg);
    if(rc != SQLITE_OK){
        wxMessageBox(wxString::Format(wxT("Ошибка сохранения:\n%s"), wxString::FromUTF8(err_msg)), wxT("Ошибка"), wxOK | wxICON_ERROR);
        sqlite3_free(err_msg);
    }
    else{
        wxMessageBox(wxT("Запись успешно сохранена!"), wxT("Успех"), wxOK | wxICON_INFORMATION);
        LoadRecord(m_id);
    }
}

void Edit_BD::OnItemActivated(wxListEvent& event){                  //для двойного нажатия
    int row = event.GetIndex();
    int col = event.GetColumn();

    wxString fieldName = m_cur_col[row];
    if(fieldName == "id" || fieldName == "ID"){
        wxMessageBox(wxT("Поле ID нельзя редактировать!"), wxT("Информация"), wxOK);
        return;
    }

    if(col == 1){
        wxMessageBox(wxT("нельзя редактировать название полей"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    wxString cur_val = m_list_edit->GetItemText(row, 1);
    wxTextEntryDialog dlg(this, wxString::Format(wxT("Изменить значение для '%s':"), m_cur_col[row]), wxT("Редактирование"), cur_val);
    if(dlg.ShowModal() == wxID_OK){
        wxString new_val = dlg.GetValue();
        m_list_edit->SetItem(row, 1, new_val);

        if(row >= 0 && row < (int)m_cur_row.size()){
            m_cur_row[row] = new_val;
        }
    }
}