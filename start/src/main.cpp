#include <wx/wx.h>

class MyApp : public wxApp{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class Start_Frame : public wxFrame{
public:
    Start_Frame(wxWindow* parent, wxString title = wxT("Менеджер баз данных"));
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
}

bool MyApp::OnInit(){
    Start_Frame* frame = new Start_Frame(NULL);          
    frame->Show(true);
    return true;
}