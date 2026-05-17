#include <wx/wx.h>
#include "sqlite3.h"
#include <iostream>
#include <vector>
#include "main.h"
#include "open_bd.h"
#include "edit_bd.h"
#include "del_bd.h"
#include "add_bd.h"
#include "create_bd.h"
#include <wx/listctrl.h>
#include <wx/filename.h>

Start_Frame::Start_Frame(wxWindow* parent, wxString title) : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)){     //wxSize - размер окна
    wxMenu *file_menu = new wxMenu();
    wxMenu *about_menu = new wxMenu();
    wxMenu *edit_menu = new wxMenu();
    
    file_menu->Append(wxID_NEW, _T("&Новая БД\tCtrl-N"), _T("Создать новую базу данных"));
    file_menu->Append(wxID_OPEN, _T("&Открыть БД\tCtrl-O"), _T("Открыть существующую БД"));
    file_menu->AppendSeparator();
    
    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, wxT("&Файл"));
    SetMenuBar(menu_bar);

    wxImage::AddHandler(new wxPNGHandler());
    
    wxBitmap new_bd(wxT("C:/devel/icons8-file-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap open(wxT("C:/devel/icons8-folders-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap add(wxT("C:/devel/icons8-plus-math-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap edit(wxT("C:/devel/icons8-создать-новый-48.png"), wxBITMAP_TYPE_PNG);
    wxBitmap del(wxT("C:/devel/icons8-explosion-48.png"), wxBITMAP_TYPE_PNG);
    
    wxToolBar *tool_bar = CreateToolBar();
    tool_bar->SetWindowStyleFlag(wxTB_TEXT);
    tool_bar->AddTool(wxID_NEW, wxT("Новая БД"), new_bd);
    tool_bar->AddTool(wxID_OPEN, wxT("Открыть БД"), open);
    tool_bar->AddSeparator();
    tool_bar->AddTool(wxID_ADD, wxT("Добавить новую запись"), add);
    tool_bar->AddTool(wxID_EDIT, wxT("Редактировать запись"), edit);
    tool_bar->AddTool(wxID_DELETE, wxT("Удалить запись"), del);
    tool_bar->Realize();

    Bind(wxEVT_MENU, &Start_Frame::OnNewBD, this, wxID_NEW);
    Bind(wxEVT_TOOL, &Start_Frame::OnNewBD, this, wxID_NEW);
    Bind(wxEVT_MENU, &Start_Frame::OnOpenBD, this, wxID_OPEN);
    Bind(wxEVT_TOOL, &Start_Frame::OnOpenBD, this, wxID_OPEN);
    Bind(wxEVT_MENU, &Start_Frame::OnAddNewBD, this, wxID_ADD);
    Bind(wxEVT_TOOL, &Start_Frame::OnAddNewBD, this, wxID_ADD);
    Bind(wxEVT_MENU, &Start_Frame::OnEditBD, this, wxID_EDIT);
    Bind(wxEVT_TOOL, &Start_Frame::OnEditBD, this, wxID_EDIT);
    Bind(wxEVT_MENU, &Start_Frame::OnDelBd, this, wxID_DELETE);
    Bind(wxEVT_TOOL, &Start_Frame::OnDelBd, this, wxID_DELETE);

    wxPanel *main_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *row1_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* cur_label = new wxStaticText(main_panel, wxID_ANY, wxT("База данных:"));
    m_dbNameLabel = new wxStaticText(main_panel, wxID_ANY, wxT("(не выбрана)"));
    m_dbNameLabel->SetForegroundColour(wxColour(100, 100, 100));
    m_table_choice = new wxChoice(main_panel, wxID_ANY);
    m_table_choice->SetMinSize(wxSize(300, -1));
    m_table_choice->Enable(false);
    row1_sizer->Add(cur_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    row1_sizer->Add(m_dbNameLabel, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    row1_sizer->AddStretchSpacer();
    row1_sizer->Add(m_table_choice, 1, wxALL | wxEXPAND, 5);
    main_sizer->Add(row1_sizer, 0, wxEXPAND | wxALL, 5);

    m_list = new wxListCtrl(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);
    main_sizer->Add(m_list, 1, wxALL | wxEXPAND, 5);
    
    main_panel->SetSizer(main_sizer);
    main_panel->Layout();

    SetSizeHints(600, 400);

    m_bd.reset(nullptr);
    m_table_choice->Bind(wxEVT_CHOICE, &Start_Frame::OnTableSelected, this);
};

void Start_Frame::OpenBD(const wxString& dbPath){
    if(dbPath.IsEmpty()){
        show_error(wxT("Путь к БД пуст"));
        return;
    }

    if(wxFileName::FileExists(dbPath) == false){
        show_error(wxT("Базы данных не существует"));
        return;
    }

    close_bd();
    
    if(open_bd(dbPath)){
        wxFileName fileName(dbPath);
        m_dbNameLabel->SetLabel(fileName.GetFullName());
        m_dbNameLabel->SetForegroundColour(wxColour(0, 150, 0));

        m_table_choice->Enable(true);
        m_table_choice->Clear();
        m_list->ClearAll();

        LoadTables();
    }
    else{
        show_error(wxT("Не удалось открыть базу данных"));
    }
}

void Start_Frame::OnNewBD(wxCommandEvent& event){
    wxDialog* dlg = new Create_BD(this);
    dlg->ShowModal();
    delete dlg;
}

void Start_Frame::OnOpenBD(wxCommandEvent& event){
    wxDialog* dlg = new Open_BD(this);
    if(dlg->ShowModal() == wxID_OK){
        // Downcast: wxDialog -> Open_BD для доступа к GetSelPath()
        Open_BD* openDlg = dynamic_cast<Open_BD*>(dlg);
        if(openDlg) {
            wxString path = openDlg->GetSelPath();
            OpenBD(path);
        }
    }
    delete dlg;
}

void Start_Frame::OnAddNewBD(wxCommandEvent& event){
    if(!m_bd){
        show_error(wxT("База данных не открыта!"));
        return;
    }
    wxString table_name = m_table_choice->GetString(event.GetSelection());
    Add_New_BD dlg(this, m_bd.get(), table_name);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        LoadTableData(table_name);
    }
}

void Start_Frame::OnEditBD(wxCommandEvent& event){
    if(!m_bd){
        show_error(wxT("База данных не открыта!"));
        return;
    }
    long selectedRow = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if(selectedRow == -1){
        show_error(wxT("Выберите запись для редактирования!"));
        return;
    }
    
    wxString idStr = m_list->GetItemText(selectedRow, 0);
    long recordId;
    if(!idStr.ToLong(&recordId)){
        show_error(wxT("Не удалось определить ID записи!"));
        return;
    }
    
    wxString table_name = m_table_choice->GetString(m_table_choice->GetSelection());
    set_table_name(table_name);

    Edit_BD dlg(this, m_bd.get(), table_name, (int)recordId);
    if(dlg.ShowModal() == wxID_OK){
        LoadTableData(table_name);  // обновляем таблицу
    }
}

void Start_Frame::OnDelBd(wxCommandEvent& event){
    if(!m_bd){
        show_error(wxT("База данных не открыта!"));
        return;
    }

    if(m_table_choice->GetCount() == 0){
        show_error(wxT("Нет таблиц для удаления записей!"));
        return;
    }

    long sel_row = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel_row == -1){
        show_error(wxT("Выберите запись для удаления!"));
        return;
    }

    wxString id = m_list->GetItemText(sel_row, 0);
    long rec_id;                                    //нужен чтоб преобразовать из строчки число, в wxListCtrl хранятся только строчки
    if(id.ToLong(&rec_id) == false){
        show_error(wxT("Не удалось определить ID записи!"));
        return;
    }

    wxString table_name = m_table_choice->GetString(event.GetSelection());
    Del_BD dlg(this, m_bd.get(), table_name, rec_id);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        LoadTableData(table_name);
    }
}

bool MyApp::OnInit(){
    wxLog::EnableLogging(false);
    wxLog::SetVerbose(false);

    Start_Frame* frame = new Start_Frame(NULL);  
    
    wxIcon icon;
    if(icon.LoadFile(wxT("C:/devel/icons8-шаблон-48.png"), wxBITMAP_TYPE_ICO)){
        frame->SetIcon(icon);
    }
    else if(icon.LoadFile(wxT("C:/devel/icons8-шаблон-48.png"), wxBITMAP_TYPE_PNG)){
        frame->SetIcon(icon);
    }

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

    if(!m_bd){
        show_error(wxT("База данных не открыта"));
        return;
    }

    if(m_list){
        m_list->ClearAll();
    }
    
    wxString sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    if((*this)(sql, GetTablesCallback, m_table_choice) != true){
        return;
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
    if(!m_bd){
        wxMessageBox("m_bd == nullptr", wxT("Ошибка"), wxOK);
        return;
    }

    if(m_list == nullptr){
        wxMessageBox("m_list == nullptr", wxT("Ошибка"), wxOK);
        return;
    }

    set_table_name(table_name);

    if(!(!(*this))){
        show_error(wxString::Format(wxT("Таблица %s не существует!"), table_name));
        return;
    }

    m_list->ClearAll();
    while(m_list->GetColumnCount() > 0){
        m_list->DeleteColumn(0);
    }

    wxString col_tab = wxString::Format("PRAGMA table_info(%s);", table_name);      //sql запрос, format - принимает строку как printf в с, PRAGMA table_info передает название колонки и какой тип данных в нем хранится и еще какую-то парашу, которая мне не особо нужна
    std::vector<wxString> col;
    if((*this)(col_tab, get_col_callback, &col) != true){
        show_error("Не удалось получить структуру таблицы");
        return;
    }

    for(size_t i = 0; i < col.size(); i++){                                         //size_t нужен для правильного сравнения размеров
        m_list->InsertColumn(i, col[i]);
        if(i == 0){
            m_list->SetColumnWidth(i, 100);
        }
        else{
            m_list->SetColumnWidth(i, wxLIST_AUTOSIZE);
        }
    }

    wxString sql = *this + "";
    if((*this)(sql, DisplayTableCallback, m_list) != true){
        return;
    }

    for(size_t i = 1; i < col.size(); i++){
        m_list->SetColumnWidth(i, wxLIST_AUTOSIZE);
        int width = m_list->GetColumnWidth(i);
        if(width < 80){
            m_list->SetColumnWidth(i, 80);
        } 
    }
}

void Start_Frame::OnTableSelected(wxCommandEvent& event){                           //ничегоне возвращает, так как обработчик события выбора бд
    wxString table_name = m_table_choice->GetString(event.GetSelection());          //возвращает строчку типа wxString с названием выбраной бд
    LoadTableData(table_name);                                                      //вызываем функцию, которая рисует табличку
}