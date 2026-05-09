#include <wx/wx.h>
#include "sqlite3.h"
#include <iostream>
#include <vector>
#include "main.h"
#include "open_bd.h"
#include "edit_bd.h"
#include "del_bd.h"
#include "add_bd.h"
#include <wx/listctrl.h>
#include <wx/filename.h>

Dialog_Create_BD::Dialog_Create_BD(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxT("Создание новой бд"), wxDefaultPosition, wxSize(400, 200)){
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
    browse_btn->Bind(wxEVT_BUTTON, &Dialog_Create_BD::OnBrowse, this);                              //подключение кнопки, она очень волшебная, я хуй знает как она работает
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

void Dialog_Create_BD::OnBrowse(wxCommandEvent& event){                                             //Открывает диалог выбора папки (wxDirDialog) Начальный путь — текущее значение из поля пути Если пользователь выбрал папку (нажал OK), обновляет поле пути
    wxDirDialog dlg(this, "Выберите папку для сохранения базы данных", m_pathBD->GetValue());
    if (dlg.ShowModal() == wxID_OK){
        m_pathBD->SetValue(dlg.GetPath());
    }
};

void Dialog_Create_BD::OnOk(wxCommandEvent& event){                                                 //Получает введённые название и путь Проверяет, что название не пустое Показывает сообщение об успехе Закрывает диалог (EndModal)
    wxString dbName = m_nameBD->GetValue();   // читаем название
    wxString dbPath = m_pathBD->GetValue();   // читаем путь
    
    if(dbName.IsEmpty()){
        wxMessageBox("Введите название базы данных!", "Ошибка", wxOK | wxICON_ERROR);
        return;
    }
    
    wxMessageBox(wxString::Format("База данных '%s' создана в папке '%s'", dbName, dbPath), "Успех", wxOK);
    
    EndModal(wxID_OK);
};

Start_Frame::Start_Frame(wxWindow* parent, wxString title) : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800, 700)){     //wxSize - размер окна
    wxMenu *file_menu = new wxMenu();                                       //создание объекта меню файл
    wxMenu *about_menu = new wxMenu();                                      //создание объекта меню справка
    wxMenu *edit_menu = new wxMenu();                                       //создание объекта меню правка
    file_menu->Append(wxID_ANY, _T("&Test\tAlt-T"), _T("Test"));            //добавляет во вкладку файл пункт тест с горячей клавишей Alt + T
    file_menu->Append(wxID_EXIT);
    about_menu->Append(wxID_ABOUT);
    
    wxMenuBar *menu_bar = new wxMenuBar();                                  //создаём саму строчку с объектами
	menu_bar->Append(file_menu, wxT("&Файл"));	                            //добавляем объект файл
    menu_bar->Append(edit_menu,wxT("&Правка"));                             //добавляем объект правка
	menu_bar->Append(about_menu, wxT("&Справка"));	                        //добавляем объект справка
	SetMenuBar(menu_bar);                                                   //добавляем меню бар на окно

    wxImage::AddHandler(new wxPNGHandler);                                  //для добавления картинок
    wxBitmap new_bd(wxT("c:/devel/icons8-папка-32.png"), wxBITMAP_TYPE_PNG);//добавление картинки для новой бд
    wxBitmap open(wxT("c:/devel/icons8-группа-папок-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap save(wxT("c:/devel/icons8-save-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap add(wxT("c:/devel/icons8-save-50.png"), wxBITMAP_TYPE_PNG);
    wxBitmap edit(wxT("c:/devel/icons8-создать-новый-16.png"), wxBITMAP_TYPE_PNG);
    wxBitmap del(wxT("c:/devel/icons8-close-48.png"), wxBITMAP_TYPE_PNG);

    wxToolBar *tool_bar = CreateToolBar();                                  //создание панели инструментов
    tool_bar->SetWindowStyleFlag(wxTB_TEXT);                                //показывает текст под иконкой
    tool_bar->AddTool(wxID_NEW, wxT("Новая БД"), new_bd);                   //добавление на нее кнопочек
    tool_bar->AddTool(wxID_OPEN, wxT("Открыть БД"), open);
    tool_bar->AddTool(wxID_SAVE, wxT("Сохранить БД"), save);
    tool_bar->AddTool(wxID_ADD, wxT("Добавить БД"), add);
    tool_bar->AddTool(wxID_EDIT, wxT("Редактировать БД"), edit);
    tool_bar->AddTool(wxID_DELETE, wxT("Удалить БД"), del);
    tool_bar->Realize();                                                    //добавление панели инструментов на окно

    Bind(wxEVT_MENU, &Start_Frame::OnNewBD, this, wxID_NEW);                //подключение кнопки новая бд
    Bind(wxEVT_TOOL, &Start_Frame::OnNewBD, this, wxID_NEW);

    Bind(wxEVT_MENU, &Start_Frame::OnOpenBD, this, wxID_OPEN);              //подключение кнопки открыть бд
    Bind(wxEVT_TOOL, &Start_Frame::OnOpenBD, this, wxID_OPEN);

    Bind(wxEVT_MENU, &Start_Frame::OnAddNewBD, this, wxID_ADD);              //подключение кнопки открыть бд
    Bind(wxEVT_TOOL, &Start_Frame::OnAddNewBD, this, wxID_ADD);

    Bind(wxEVT_MENU, &Start_Frame::OnEditBD, this, wxID_EDIT);              //подключение кнопки открыть бд
    Bind(wxEVT_TOOL, &Start_Frame::OnEditBD, this, wxID_EDIT);

    Bind(wxEVT_MENU, &Start_Frame::OnDelBd, this, wxID_DELETE);              //подключение кнопки открыть бд
    Bind(wxEVT_TOOL, &Start_Frame::OnDelBd, this, wxID_DELETE);
    //расположение всего на экране 
    wxPanel *main_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);   //wxTAB_TRAVERSAL — позволяет переключаться между элементами клавишей Tab
    wxBoxSizer *VStart_Frame = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *HFrame_Control1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control4 = new wxBoxSizer(wxHORIZONTAL);

    m_bd = nullptr;
    m_list = new wxListCtrl(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);

    wxStaticText* cur_label = new wxStaticText(main_panel, wxID_ANY, wxT("текущая база данных"));
    HFrame_Control1->Add(cur_label);

    wxStaticText* name_label = new wxStaticText(main_panel, wxID_ANY, wxT("поиск"));
    m_findBD = new wxTextCtrl(main_panel, wxID_ANY);
    wxButton* btn_find = new wxButton(main_panel, wxID_ANY, wxT("Найти"));
    wxButton* btn_clear = new wxButton(main_panel, wxID_ANY, wxT("Сброс"));
    HFrame_Control2->Add(name_label);
    HFrame_Control2->Add(m_findBD);
    HFrame_Control2->Add(btn_find);
    HFrame_Control2->Add(btn_clear);

    m_table_choice = new wxChoice(main_panel, wxID_ANY);
    HFrame_Control3->Add(m_table_choice);

    wxButton* btn_plus = new wxButton(main_panel, wxID_ANY, wxT("добавить"));
    wxButton* btn_edit = new wxButton(main_panel, wxID_ANY, wxT("редактировать"));
    wxButton* btn_del = new wxButton(main_panel, wxID_ANY, wxT("удалить"));
    wxButton* btn_reset = new wxButton(main_panel, wxID_ANY, wxT("обновить"));

    HFrame_Control4->Add(btn_plus);
    HFrame_Control4->Add(btn_edit);
    HFrame_Control4->Add(btn_del);
    HFrame_Control4->Add(btn_reset);

    VStart_Frame->Add(HFrame_Control1);
    VStart_Frame->Add(HFrame_Control2);
    VStart_Frame->Add(HFrame_Control3);
    VStart_Frame->Add(m_list);
    VStart_Frame->Add(HFrame_Control4);

    main_panel->SetSizer(VStart_Frame);
    main_panel->Layout();
};

void Start_Frame::OpenBD(const wxString& dbPath){
    if(dbPath.IsEmpty()){
        wxMessageBox(wxT("путь к бд пуст"), wxT("ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    if(wxFileName::FileExists(dbPath) == false){
        wxMessageBox(wxT("бд не существует"), wxT("ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    if(m_bd){
        sqlite3_close(m_bd);
        m_bd = nullptr;
    }
    
    int result = sqlite3_open(dbPath.ToUTF8(), &m_bd);

    if(sqlite3_open(dbPath.ToUTF8(), &m_bd) != SQLITE_OK){
        wxMessageBox(wxT("Не удалось открыть базу данных"), wxT("ошибка"), wxOK | wxICON_ERROR);
        m_bd = nullptr;
        return;
    }
    
    //SetStatusText("Открыта БД: " + dbPath);
    LoadTables();
}

void Start_Frame::OnNewBD(wxCommandEvent& event){
    Dialog_Create_BD dlg(this);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его

    }
}

void Start_Frame::OnOpenBD(wxCommandEvent& event){
    Open_BD dlg(this);                              // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        wxString path = dlg.GetSelPath();
        OpenBD(path);
        /*if(path.IsEmpty() == false){
            OpenBD(path);
        }*/
    }
}

void Start_Frame::OnAddNewBD(wxCommandEvent& event){
    Add_new_BD dlg(this);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        
    }
}

void Start_Frame::OnEditBD(wxCommandEvent& event){
    if(m_bd == nullptr){
        wxMessageBox(wxT("База данных не открыта!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    wxString table_name = m_table_choice->GetString(event.GetSelection());
    Edit_BD dlg(this, m_bd, table_name);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){
        LoadTableData(table_name);
    }
}

void Start_Frame::OnDelBd(wxCommandEvent& event){
    if(m_bd == nullptr){
        wxMessageBox(wxT("База данных не открыта!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    if(m_table_choice->GetCount() == 0){
        wxMessageBox(wxT("Нет таблиц для удаления записей!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    long sel_row = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel_row == -1){
        wxMessageBox(wxT("Выберите запись для удаления!"), wxT("Информация"), wxOK | wxICON_INFORMATION);
        return;
    }

    wxString id = m_list->GetItemText(sel_row, 0);
    long rec_id;                                    //нужен чтоб преобразовать из строчки число, в wxListCtrl хранятся только строчки
    if(id.ToLong(&rec_id) == false){
        wxMessageBox(wxT("Не удалось определить ID записи!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    wxString table_name = m_table_choice->GetString(event.GetSelection());
    Del_BD dlg(this, m_bd, table_name, rec_id);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        LoadTableData(table_name);
    }
}

bool MyApp::OnInit(){
    Start_Frame* frame = new Start_Frame(NULL);          
    frame->Show(true);
    return true;
}

static int GetTablesCallback(void* data, int arg_c, char** arg_v, char** azColName){        //добавляет название таблицы в выпадающий список
    wxChoice* choice = (wxChoice*)data;
    if(arg_c > 0 && arg_v[0]){                                                  //проверяем не пустая ли бд 
        choice->Append(wxString::FromUTF8(arg_v[0]));
    }
    return 0;
}

static int DisplayTableCallback(void* data, int arg_c, char** arg_v, char** azColName){
    if(data == nullptr){
        wxMessageBox(wxT("DisplayTableCallback: data == nullptr"), wxT("Ошибка"), wxOK);
        return 1;
    }

    wxListCtrl* list = (wxListCtrl*)data;

    if(list == nullptr){
        wxMessageBox(wxT("DisplayTableCallback: list == nullptr"), wxT("Ошибка"), wxOK);
        return 1;
    }                                                                           

    long item_index = list->GetItemCount();                                     //номер новой строки
    for(int i = 0; i < arg_c; i++){
        wxString val;
        if(arg_v[i] != nullptr){
            val = wxString::FromUTF8(arg_v[i]);
        }
        else{
            val = "";
        }

        if(i == 0){
            list->InsertItem(item_index, val);                                  //создаем глвую строчку
        }
        else{
            list->SetItem(item_index, i, val);                                  //заполняем значением только что созданную строчку
        }
    }
    return 0;
}

void Start_Frame::LoadTables(){
    m_table_choice->Clear();

    if(m_bd == nullptr){
        wxMessageBox(wxT("LoadTables: m_bd == nullptr"), wxT("Ошибка"), wxOK);
        return;
    }

    if(m_list){
        m_list->ClearAll();
    }

    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_bd, "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';", GetTablesCallback, m_table_choice, &err_msg);         //вызывает GetTablesCallback для каждой найденной таблицы

    if(rc != SQLITE_OK){
        wxMessageBox(wxString::FromUTF8(err_msg), wxT("ошибка"), wxOK | wxICON_ERROR);
        sqlite3_free(err_msg);
    }

    if(m_table_choice->GetCount() > 0){
        m_table_choice->SetSelection(0);                        //выбираем первую
        LoadTableData(m_table_choice->GetString(0));            //выгружаем ее данные
    }
}

static int get_col_callback(void* data, int arg_c, char** arg_v, char** az_col_name){
    std::vector<wxString>* col = static_cast<std::vector<wxString>*>(data);      //принудительно приводим к типу данных, В круглых скобках — значение value, в угловых скобках — тип type.
    if(arg_c > 1 && arg_v[1]){
        col->push_back(wxString::FromUTF8(arg_v[1]));                            //FromUTF8 преобразует с-строку в объект wxString, 1 возвращает имя колонки
    }
    return 0;
}

void Start_Frame::LoadTableData(const wxString& table_name/*надо где-то принимать с клавы*/){
    if(m_bd == nullptr){
        wxMessageBox("m_bd == nullptr", wxT("Ошибка"), wxOK);
        return;
    }

    if(m_list == nullptr){
        wxMessageBox("m_list == nullptr", wxT("Ошибка"), wxOK);
        return;
    }

    m_list->ClearAll();
    while(m_list->GetColumnCount() > 0){
        m_list->DeleteColumn(0);
    }
    //char* escaped = sqlite3_mprintf("%w", tableName.ToUTF8());        для ввода пользователем, надо кудато присобачить
    wxString col_tab = wxString::Format("PRAGMA table_info(%s);", table_name);      //sql запрос, format - принимает строку как printf в с, PRAGMA table_info передает название колонки и какой тип данных в нем хранится и еще какую-то парашу, которая мне не особо нужна
    std::vector<wxString> col;
    int rc = sqlite3_exec(m_bd, col_tab.ToUTF8(), get_col_callback, &col, nullptr);          //заполняется вектор col названиями столбцов бд
    if(rc != SQLITE_OK){
        wxMessageBox(wxT("Не удалось получить структуру таблицы"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }

    for(size_t i = 0; i < col.size(); i++){                                         //size_t нужен для правильного сравнения размеров
        m_list->InsertColumn(i, col[i]);
        m_list->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);                       //ширина всего столбца такая, чтоб полностью влезало название столбца
    }

    wxString table = wxString::Format("SELECT * FROM %s", table_name);              //забираем все данные из таблицы, за исключением названия столбцов
    
    char* err_msg = nullptr;
    sqlite3_exec(m_bd, table.ToUTF8(), DisplayTableCallback, m_list, &err_msg);

    //SetStatusText(wxString::Format("записей: %s", m_list->GetItemCount()));         //добавляет строчку "было добавлено столько-то записей"
}

void Start_Frame::OnTableSelected(wxCommandEvent& event){                           //ничегоне возвращает, так как обработчик события выбора бд
    wxString table_name = m_table_choice->GetString(event.GetSelection());          //возвращает строчку типа wxString с названием выбраной бд
    LoadTableData(table_name);                                                      //вызываем функцию, которая рисует табличку
}