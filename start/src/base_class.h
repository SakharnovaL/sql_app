#ifndef BASE_CLASS_H
#define BASE_CLASS_H

#include <wx/wx.h>
#include <sqlite3.h>
#include <wx/filename.h>
#include <wx/bitmap.h>
#include <wx/artprov.h>
#include <wx/button.h>
#include <memory>

class Base_Class{
protected:
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_bd{nullptr, sqlite3_close};
    wxString m_pathBD;
    wxString m_table_name;
    bool m_success;
    bool m_isolated;  // Флаг изоляции
    
public:
    Base_Class() : m_bd(nullptr, sqlite3_close), m_success(false), m_isolated(false){};
    
    // Конструктор для изолированных диалогов
    Base_Class(bool isolated) : m_bd(nullptr, sqlite3_close), m_success(false), m_isolated(isolated){};

    virtual bool open_bd(const wxString& path){
        m_pathBD = path;
        sqlite3* raw_ptr = nullptr;
        if(sqlite3_open(path.ToUTF8(), &raw_ptr) == SQLITE_OK){
            m_bd.reset(raw_ptr);
            m_success = true;
            return true;
        }
        else{
            m_success = false;
            return false;
        }
    }

    void close_bd(){
        if(m_bd){
            m_bd.reset();
            m_success = false;
        }
    }

    sqlite3* get_bd() const {return m_bd.get();}
    wxString get_bdPath() const {return m_pathBD;}
    wxString get_table_name() const {return m_table_name;}
    bool get_succsess() const {return m_success;}

    void set_bd(sqlite3* bd){
        if(m_bd.get() != bd){
            m_bd.reset(bd);
        }
    }
    
    void set_table_name(const wxString& name){
        if(!m_isolated){  // Только не изолированные объекты могут изменять имя
            m_table_name = name;
        }
    }
    
    void set_succsess(bool succsess){m_success = succsess;}

    void show_error(const wxString& err_msg, const wxString& title = wxT("Ошибка"), 
                    const wxString& icon_path = wxT("C:/devel/icons8-error-48.png")){
        wxDialog error_dlg(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 150));
        
        wxPanel* panel = new wxPanel(&error_dlg, wxID_ANY);
        wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* content_sizer = new wxBoxSizer(wxHORIZONTAL);
        
        if(!icon_path.IsEmpty() && wxFileName::FileExists(icon_path)){
            wxBitmap bitmap(icon_path, wxBITMAP_TYPE_PNG);
            if(bitmap.IsOk()){
                wxStaticBitmap* icon = new wxStaticBitmap(panel, wxID_ANY, bitmap);
                content_sizer->Add(icon, 0, wxALL | wxALIGN_CENTER, 10);
            }
        }
        else{
            wxBitmap default_bmp = wxArtProvider::GetBitmap(wxART_ERROR, wxART_MESSAGE_BOX);
            wxStaticBitmap* icon = new wxStaticBitmap(panel, wxID_ANY, default_bmp);
            content_sizer->Add(icon, 0, wxALL | wxALIGN_CENTER, 10);
        }
        
        wxStaticText* text = new wxStaticText(panel, wxID_ANY, err_msg);
        text->Wrap(350);
        text->SetForegroundColour(wxColour(200, 0, 0));
        wxFont font = text->GetFont();
        font.SetPointSize(10);
        text->SetFont(font);
        content_sizer->Add(text, 1, wxALL | wxALIGN_CENTER_VERTICAL, 10);
        
        main_sizer->Add(content_sizer, 1, wxEXPAND | wxALL, 10);
        
        wxButton* ok_btn = new wxButton(panel, wxID_OK, wxT("OK"));
        wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
        btn_sizer->Add(ok_btn, 0, wxALL | wxALIGN_CENTER, 10);
        main_sizer->Add(btn_sizer, 0, wxALIGN_CENTER);
        
        panel->SetSizer(main_sizer);
        error_dlg.SetMinSize(wxSize(400, 150));
        error_dlg.ShowModal();
    }

    bool make_sql(const wxString& sql, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr){
        if(!m_bd){
            show_error(wxT("База данных не открыта!"));
            return false;
        }
        char* err_msg = nullptr;
        int rc = sqlite3_exec(m_bd.get(), sql.ToUTF8(), callback, data, &err_msg);
        if(rc != SQLITE_OK){
            wxString error_msg = wxString::FromUTF8(err_msg);
            show_error(wxString::Format(wxT("SQL ошибка: %s"), error_msg));
            sqlite3_free(err_msg);
            return false;
        }
        return true;
    }

    bool operator()(const wxString& sql, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr){
        return make_sql(sql, callback, data);
    }
    
    wxString operator+(const wxString& sql_part) const {
        if(m_table_name.IsEmpty()){
            return wxT("");
        }
        if(sql_part.IsEmpty()){
            return wxString::Format("SELECT * FROM %s", m_table_name);
        }
        return wxString::Format("SELECT * FROM %s %s", m_table_name, sql_part);
    }
    
    bool operator!() {
        if(m_table_name.IsEmpty()) return false;
        
        wxString sql = wxString::Format("SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", m_table_name);
        sqlite3_stmt* stmt;
        bool exists = false;
        
        if(sqlite3_prepare_v2(m_bd.get(), sql.ToUTF8(), -1, &stmt, nullptr) == SQLITE_OK){
            if(sqlite3_step(stmt) == SQLITE_ROW){
                exists = true;
            }
            sqlite3_finalize(stmt);
        }
        
        return exists;
    }
};

#endif