#include <wx/wx.h>

class MyApp : public wxApp{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class Dialog_Create_BD : public wxDialog{
public:
    Dialog_Create_BD(wxWindow* parent);         //создание самого диалогового окна
    wxTextCtrl* m_nameBD;                       //поле для ввода названия
    wxTextCtrl* m_pathBD;                       //поле для ввода пути к бд

    void OnOk(wxCommandEvent& event);           //отработка кнопки создать
    void OnBack(wxCommandEvent& event);         //отработка кнопки отмена
    void OnBrowse(wxCommandEvent& evenr);       //отработка кнопки обзор для выбора пути 
};

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

class Start_Frame : public wxFrame{
public:
    Start_Frame(wxWindow* parent, wxString title = wxT("Менеджер баз данных"));
    void OnNewBD(wxCommandEvent& event);
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
    //расположение всего на экране
    wxPanel *main_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);   //wxTAB_TRAVERSAL — позволяет переключаться между элементами клавишей Tab
    wxBoxSizer *VStart_Frame = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *HFrame_Control1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *HFrame_Control4 = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btn_find = new wxButton(main_panel, wxID_ANY, wxT("Найти"));
    wxButton* btn_clear = new wxButton(main_panel, wxID_ANY, wxT("Сброс"));

    HFrame_Control2->Add(btn_find);
    HFrame_Control2->Add(btn_clear);

    VStart_Frame->Add(HFrame_Control1);
    VStart_Frame->Add(HFrame_Control2);
    VStart_Frame->Add(HFrame_Control3);
    VStart_Frame->Add(HFrame_Control4);

    main_panel->SetSizer(VStart_Frame);
    main_panel->Layout();
};

void Start_Frame::OnNewBD(wxCommandEvent& event){
    Dialog_Create_BD dlg(this);                     // создаём диалог
    if(dlg.ShowModal() == wxID_OK){                 // показываем его
        SetStatusText("База данных создана");       // обновляем статус
    }
}

bool MyApp::OnInit(){
    Start_Frame* frame = new Start_Frame(NULL);          
    frame->Show(true);
    return true;
}