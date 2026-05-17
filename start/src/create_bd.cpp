#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/listctrl.h>
#include "create_bd.h"
#include <wx/filename.h>
#include <vector>

Create_BD::Create_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Создание новой бд"), wxDefaultPosition, wxSize(400, 300)), m_nameBD(nullptr), m_pathBD(nullptr), m_list_create(nullptr){
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);                                            //создание общего бокса куда запихаем все состовляющие окна

    wxBoxSizer* name_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* name_label = new wxStaticText(panel, wxID_ANY, wxT("введите название бд:"));      //статичный текст 
    m_nameBD = new wxTextCtrl(panel, wxID_ANY, wxT("новая база"));                                  //поле куда можно ввести название бд, по умолчанию - новая база
    name_sizer->Add(name_label);
    name_sizer->Add(m_nameBD);
    main_sizer->Add(name_sizer, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* path_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* path_lable = new wxStaticText(panel, wxID_ANY, wxT("выберете путь:"));            //статичный текст
    m_pathBD = new wxTextCtrl(panel, wxID_ANY, wxT("C:/devel/start/tables"));                                         //поле куда можно вписать путь бд, wxGetCwd() возвращает текущий рабочий каталог - значение по умолчанию
    wxButton* browse_btn = new wxButton(panel, wxID_ANY, wxT("обзор"));
    browse_btn->Bind(wxEVT_BUTTON, &Create_BD::OnBrowse, this);                              //подключение кнопки, она очень волшебная, я хуй знает как она работает
    path_sizer->Add(path_lable);
    path_sizer->Add(m_pathBD);
    path_sizer->Add(browse_btn);
    main_sizer->Add(path_sizer, 0, wxALL | wxEXPAND);

    m_list_create = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);
    m_list_create->InsertColumn(0, wxT("id"));
    m_col_name.clear();
    m_col_name.push_back("id");
    m_list_create->Bind(wxEVT_LIST_ITEM_ACTIVATED, &Create_BD::OnCellEdit, this);
    m_list_create->Bind(wxEVT_LIST_COL_CLICK, &Create_BD::OnColEdit, this);
    main_sizer->Add(m_list_create, 1, wxALL | wxEXPAND);

    wxBoxSizer* control_btn = new wxBoxSizer(wxHORIZONTAL);
    wxButton* add_col = new wxButton(panel, wxID_ANY, wxT("Добавить колонку"));
    wxButton* add_row = new wxButton(panel, wxID_ANY, wxT("Добавить строку"));
    add_col->Bind(wxEVT_BUTTON, &Create_BD::OnAddCol, this);
    add_row->Bind(wxEVT_BUTTON, &Create_BD::OnAddRow, this);
    control_btn->Add(add_col, 0, wxRIGHT, 5);
    control_btn->Add(add_row, 0, wxRIGHT, 5);
    main_sizer->Add(control_btn, 0, wxALL, 5);

    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("создать"));
    ok_btn->Bind(wxEVT_BUTTON, &Create_BD::OnOk, this);
    wxButton* cancel_btn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btn_sizer->Add(ok_btn);
    btn_sizer->Add(cancel_btn);
    main_sizer->Add(btn_sizer);

    panel->SetSizer(main_sizer);
};

void Create_BD::OnBrowse(wxCommandEvent& event){                                             //Открывает диалог выбора папки (wxDirDialog) Начальный путь — текущее значение из поля пути Если пользователь выбрал папку (нажал OK), обновляет поле пути
    wxDirDialog dlg(this, wxT("Выберите папку для сохранения базы данных"), m_pathBD->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if(dlg.ShowModal() == wxID_OK){
        m_pathBD->SetValue(dlg.GetPath());
    }
};

void Create_BD::OnAddCol(wxCommandEvent& event){
    wxTextEntryDialog dlg(this, wxT("Введите название новой колонки:"), wxT("Добавление колонки"), wxT("новое_поле"));
    if(dlg.ShowModal() == wxID_OK){
        wxString col_name = dlg.GetValue();
        if(col_name.IsEmpty() == false){
            int col_cnt = m_list_create->GetColumnCount();
            m_list_create->InsertColumn(col_cnt, col_name);
            m_list_create->SetColumnWidth(col_cnt, 100);
            m_col_name.push_back(col_name);
            int rowCount = m_list_create->GetItemCount();

            for(int row = 0; row < m_list_create->GetItemCount(); row++){
                m_list_create->SetItem(row, col_cnt, "");
            }
        }
    }
}

void Create_BD::OnAddRow(wxCommandEvent& event){
    int row_cnt = m_list_create->GetItemCount();
    int col_cnt = m_list_create->GetColumnCount();

    int new_row_num = row_cnt + 1;
    long new_index = m_list_create->InsertItem(row_cnt, wxString::Format("%d", new_row_num));

    for(int col = 1; col < col_cnt; col++){
        m_list_create->SetItem(new_index, col, "");
    }
}

void Create_BD::OnCellEdit(wxListEvent& event){
    int row = event.GetIndex();
    int col = event.GetColumn();
    
    if(col == 0){
        show_error(wxT("Поле ID генерируется автоматически"));
        return;
    }

    if(row < 0 || row >= m_list_create->GetItemCount()) return;

    wxString idStr = m_list_create->GetItemText(row, 0);
    
    wxDialog editDialog(this, wxID_ANY, wxT("Редактирование записи"), wxDefaultPosition, wxSize(400, 300));
    
    wxPanel* panel = new wxPanel(&editDialog, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxListCtrl* fieldList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    fieldList->InsertColumn(0, wxT("поле"));
    fieldList->InsertColumn(1, wxT("значение"));
    fieldList->SetColumnWidth(0, 120);
    fieldList->SetColumnWidth(1, 200);
    
    int colCount = m_list_create->GetColumnCount();
    for(int c = 0; c < colCount; c++){
        wxString fieldName;
        if(c < (int)m_col_name.size()){
            fieldName = m_col_name[c];
        } else {
            wxListItem item;
            item.SetMask(wxLIST_MASK_TEXT);
            item.SetId(c);
            m_list_create->GetColumn(c, item);
            fieldName = item.GetText();
        }
        
        wxString fieldValue = m_list_create->GetItemText(row, c);
        
        long index = fieldList->InsertItem(c, fieldName);
        fieldList->SetItem(index, 1, fieldValue);

         if(fieldName == "id" || fieldName == "ID"){
            fieldList->SetItemBackgroundColour(index, wxColour(230, 230, 230));
        }
    }
    
    mainSizer->Add(fieldList, 1, wxALL | wxEXPAND, 10);

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* saveBtn = new wxButton(panel, wxID_OK, wxT("сохранить"));
    wxButton* cancelBtn = new wxButton(panel, wxID_CANCEL, wxT("отмена"));
    btnSizer->AddStretchSpacer();
    btnSizer->Add(saveBtn, 0, wxRIGHT, 5);
    btnSizer->Add(cancelBtn, 0);
    mainSizer->Add(btnSizer, 0, wxALL | wxEXPAND, 10);
    
    panel->SetSizer(mainSizer);

    fieldList->Bind(wxEVT_LIST_ITEM_ACTIVATED, [fieldList, row, this](wxListEvent& e){
        int r = e.GetIndex();
        wxString cur_val = fieldList->GetItemText(r, 1);
        wxString fieldName = fieldList->GetItemText(r, 0);
        
        if(fieldName == "id" || fieldName == "ID"){
            show_error(wxT("Поле ID нельзя редактировать!"));
            return;
        }

        wxTextEntryDialog dlg(nullptr, wxString::Format(wxT("Введите значение для поля '%s':"), fieldName), wxT("Редактирование"), cur_val);
        if(dlg.ShowModal() == wxID_OK){
            fieldList->SetItem(r, 1, dlg.GetValue());
        }
    });
    
    if(editDialog.ShowModal() == wxID_OK){
        for(int c = 0; c < colCount; c++){
            wxString fieldName;
            if(c < (int)m_col_name.size()){
                fieldName = m_col_name[c];
            } else {
                wxListItem item;
                item.SetMask(wxLIST_MASK_TEXT);
                item.SetId(c);
                m_list_create->GetColumn(c, item);
                fieldName = item.GetText();
            }
            
            if(fieldName == "id" || fieldName == "ID"){
                continue;
            }
            
            wxString newValue = fieldList->GetItemText(c, 1);
            m_list_create->SetItem(row, c, newValue);
        }
        m_list_create->RefreshItem(row);
    }
}

void Create_BD::OnColEdit(wxListEvent& event){
    int col = event.GetColumn();
    if(col == 0){
        show_error(wxT("Нельзя переименовать колонку ID!"));
        return;
    }

    wxString cur_name = m_col_name[col];

    wxTextEntryDialog dlg(this, wxString::Format(wxT("Введите новое название для колонки:"), cur_name), wxT("Переименование колонки"), cur_name);
    if(dlg.ShowModal() == wxID_OK){
        wxString new_name = dlg.GetValue();
        if(new_name.IsEmpty() == false){
            m_col_name[col] = new_name;
            wxListItem newItem;
            newItem.SetMask(wxLIST_MASK_TEXT);
            newItem.SetText(new_name);
            m_list_create->SetColumn(col, newItem);
            
            m_list_create->SetColumnWidth(col, wxLIST_AUTOSIZE_USEHEADER);
        }
    }
}

std::vector<wxString> Create_BD::GetColumnNAme(){
    std::vector<wxString> col;
    int col_cnt = m_list_create->GetColumnCount();

    for(int i = 1; i < col_cnt; i++){
        if(i < (int)m_col_name.size()){
            col.push_back(m_col_name[i]);
        }
        else{
            wxListItem item;
            item.SetMask(wxLIST_MASK_TEXT);
            item.SetId(i);
            item.SetColumn(i);
            m_list_create->GetColumn(i, item);
            col.push_back(item.GetText());
        }
    }
    return col;
}

std::vector<std::vector<wxString>> Create_BD::GetTableData(){
    std::vector<std::vector<wxString>> data;
    int row_cnt = m_list_create->GetItemCount();
    int col_cnt = m_list_create->GetColumnCount();

    for(int row = 0; row < row_cnt; row++){
        std::vector<wxString> row_data;
        for(int col = 1; col < col_cnt; col++){
            row_data.push_back(m_list_create->GetItemText(row, col));
        }
        data.push_back(row_data);
    }
    return data;
}

bool Create_BD::CreateBD(){
    wxString bd_name = m_nameBD->GetValue();
    wxString bd_path = m_pathBD->GetValue();

    wxString full_path = wxString::Format("%s/%s.db", bd_path, bd_name);
    wxString table_name = wxT("my_table");
    
    //int rc = open_bd(full_path);
    if(open_bd(full_path) == false){
        show_error(wxString::Format(wxT("Ошибка создания БД: %s"), full_path));
        return false;
    }
    
    m_bdpath = full_path;

    std::vector<wxString> columns = GetColumnNAme();
    auto data = GetTableData();

    if(columns.empty()){
        show_error(wxT("Нет колонок для создания таблицы!"));
        close_bd();
        return false;
    }

    wxString sql = "CREATE TABLE " + table_name + " (id INTEGER PRIMARY KEY AUTOINCREMENT";
    for(const auto& col : columns){
        sql += wxString::Format(", %s TEXT", col);
    }
    sql += ");";

    //char* err_msg = nullptr;
    //rc = sqlite3_exec(m_bd, sql.ToUTF8(), nullptr, nullptr, &err_msg);
    if((*this)(sql, nullptr, nullptr) != true){
        show_error(wxString::Format(wxT("Ошибка создания таблицы:\n%s"), sql));
        close_bd();
        return false;
    }

    if(data.empty()){
        wxMessageBox(wxString::Format(wxT("База данных успешно создана!\n\nФайл: %s\nКолонок: %d\nЗаписей: 0"), full_path, (int)columns.size()), wxT("Успех"), wxOK);
        close_bd();
        return true;
    }

    wxString col_list;
    for(const auto& col : columns){
        if(col_list.IsEmpty() == false){
            col_list += ", ";
        }
        col_list += col;
    }

    for(const auto& row : data){
        if(row.empty()){
            continue;
        }

        wxString values;
        for(const auto& val : row){
            if(values.IsEmpty() == false){
                values += ", ";
            }
            wxString escaped = val;
            escaped.Replace("'", "''");
            values += wxString::Format("'%s'", escaped);
        }

        wxString insert_sql = wxString::Format("INSERT INTO %s (%s) VALUES (%s);", table_name, col_list, values);
        //rc = sqlite3_exec(m_bd, insert_sql.ToUTF8(), nullptr, nullptr, &err_msg);
        if((*this)(insert_sql, nullptr, nullptr) != true){
            show_error(wxString::Format(wxT("Ошибка создания таблицы:\n%s"), insert_sql));
        }
    }

    wxMessageBox(wxString::Format(wxT("База данных успешно создана!\n\n" "Файл: %s\n" "Колонок: %d\n" "Записей: %d"), full_path, (int)columns.size(), (int)data.size()), wxT("Успех"), wxOK);
    return true;
}

void Create_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    if(CreateBD()){
        EndModal(wxID_OK);
    }
};