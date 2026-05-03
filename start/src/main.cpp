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
}

bool MyApp::OnInit(){
    Start_Frame* frame = new Start_Frame(NULL);          
    frame->Show(true);
    return true;
}