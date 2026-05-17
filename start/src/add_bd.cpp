#include "add_bd.h"
#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>

Add_New_BD::Add_New_BD(wxWindow* parent, sqlite3* bd, const wxString& table_name) : wxDialog(parent, wxID_ANY, wxT("добавление новой записи"), wxDefaultPosition, wxSize(500, 400)),  m_id(-1), m_col(){
    set_bd(bd);
    set_table_name(table_name);
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* text_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* text_label = new wxStaticText(panel, wxID_ANY, wxT("введите данные новой записи"));
    text_sizer->Add(text_label);
    main_sizer->Add(text_sizer, 0, wxALL | wxALIGN_CENTER, 10);

    m_list_add = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
    m_list_add->InsertColumn(0, wxT("поле"));
    m_list_add->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    m_list_add->InsertColumn(1, wxT("значение"));
    m_list_add->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    main_sizer->Add(m_list_add, 1, wxALL | wxEXPAND, 10);

    LoadTableStruct();

    long idIndex = m_list_add->InsertItem(0, wxT("id"));
    m_list_add->SetItem(idIndex, 1, wxString::Format("%d", m_nextId));
    m_list_add->SetItemBackgroundColour(idIndex, wxColour(240, 240, 240));

    m_list_add->Bind(wxEVT_LIST_ITEM_ACTIVATED, &Add_New_BD::OnItemActivated, this);

    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("сохранить"));
    ok_btn->Bind(wxEVT_BUTTON, &Add_New_BD::OnOk, this);
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn, 0, wxRIGHT, 5);
    btn_sizer->Add(cancel_btn, 0);   
    main_sizer->Add(btn_sizer, 0, wxEXPAND | wxALL, 10);

    panel->SetSizer(main_sizer);
};

int Add_New_BD::GetNextId(){
    int maxId = 0;
    wxString sql = wxString::Format("SELECT MAX(id) FROM %s;", get_table_name());
    
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(get_bd(), sql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            maxId = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    return maxId + 1;
}

void Add_New_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    AddRecord();
    set_succsess(true);
    EndModal(wxID_OK);
};

void Add_New_BD::LoadTableStruct(){
    m_nextId = GetNextId();
    wxString col_tab = wxString::Format("PRAGMA table_info(%s);", get_table_name());

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(get_bd(), col_tab.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
        while(sqlite3_step(stmt) == SQLITE_ROW){
            const char* col_name = (const char*)sqlite3_column_text(stmt, 1);
            if(col_name){
                wxString col = wxString::FromUTF8(col_name);
                if(col != "id" && col != "ID"){
                    m_cur_col.push_back(col);
                    m_cur_row.push_back("");

                    long item_index = m_list_add->GetItemCount();
                    m_list_add->InsertItem(item_index, col);
                    m_list_add->SetItem(item_index, 1, "");
                }
            }
        }
        sqlite3_finalize(stmt);
    }
}

void Add_New_BD::OnItemActivated(wxListEvent& event){                  //для двойного нажатия
    int row = event.GetIndex();
    int col = event.GetColumn();

    wxString fieldName = m_list_add->GetItemText(row, 0);
    if(fieldName == "id" || fieldName == "ID"){
        show_error(wxT("Поле ID генерируется автоматически"));
        return;
    }

    if(col == 0){
        show_error(wxT("нельзя редактировать название полей"));
        return;
    }
    wxString cur_val = m_list_add->GetItemText(row, 1);
    wxTextEntryDialog dlg(this, wxString::Format(wxT("Изменить значение для '%s':"), fieldName), wxT("Редактирование"), cur_val);
    if(dlg.ShowModal() == wxID_OK){
        wxString new_val = dlg.GetValue();
        m_list_add->SetItem(row, 1, new_val);

        int data_index = row - 1;
        if(data_index >= 0 && data_index< (int)m_cur_row.size()){
            m_cur_row[data_index] = new_val;
        }
        else{
        show_error(wxT("Редактировать можно только поле 'Значение'"));
        }   
    }
}

void Add_New_BD::AddRecord(){
    if(m_cur_col.empty()){
        show_error(wxT("Нет полей для добавления!"));
        return;
    }

    bool all_filed = true;
    wxString empty_field;
    for(size_t i = 0; i < m_cur_col.size(); i++){
        if(m_cur_row[i].IsEmpty()){
            all_filed = false;
            empty_field += m_cur_col[i] + "\n";
        }
    }

    if(all_filed == false){
        int answer = wxMessageBox(wxString::Format(wxT("Следующие поля не заполнены:\n%s\nПродолжить?"), empty_field), wxT("Предупреждение"), wxYES_NO | wxICON_WARNING);
        if(answer != wxYES){
            return;
        }
    }

    wxString val;
    wxString col;
    for(size_t i = 0; i < m_cur_col.size(); i++){
        if(col.IsEmpty() == false){
            col += ", ";
            val += ", ";
        }
        col += m_cur_col[i];
        wxString vals = m_cur_row[i];
        vals.Replace("'", "''");
        val += wxString::Format("'%s'", vals);
    }

    wxString sql = wxString::Format("INSERT INTO %s (%s) VALUES (%s);", get_table_name(), col, val);
    if((*this)(sql, nullptr, nullptr) != true){
        show_error(wxString::Format(wxT("Ошибка добавления записи:\n%s")));
        set_succsess(false);
    } 
    else{
        wxMessageBox(wxT("Запись успешно добавлена!"), wxT("Успех"), wxOK | wxICON_INFORMATION);
        set_succsess(true);
    }
}