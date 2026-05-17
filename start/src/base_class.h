#ifndef BASE_CLASS_H
#define BASE_CLASS_H

#include <wx/wx.h>
#include <sqlite3.h>

class Base_Class{
protected:
    sqlite3* m_bd;
    wxString m_pathBD;
    wxString m_table_name;
    bool m_success;
public:
    Base_Class() : m_bd(nullptr), m_success(false){};

    virtual bool open_bd(const wxString& path){
        m_pathBD = path;
        if(sqlite3_open(path.ToUTF8(), &m_bd) == SQLITE_OK){
            return true;
        }
        else{
            return false;
        }
    }

    void close_bd(){
        if(m_bd != nullptr){
            sqlite3_close(m_bd);
        }
        m_bd = nullptr;
    }

    sqlite3* get_bd() const {return m_bd;}
    wxString get_bdPath() const {return m_pathBD;}
    wxString get_table_name() const {return m_table_name;}
    bool get_succsess() const {return m_success;}

    void set_bd(sqlite3* bd){m_bd = bd;}
    void set_table_name(const wxString& name){m_table_name = name;}
    void set_succsess(bool succsess){m_success = succsess;}

    void show_error(const wxString& err_msg){
        wxMessageBox(err_msg, wxT("Ошибка"), wxOK | wxICON_ERROR);
    }

    bool make_sql(const wxString& sql, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr){
        if(m_bd == nullptr){
            show_error(wxT("База данных не открыта!"));
            return false;
        }
        char* err_msg = nullptr;
        int rc = sqlite3_exec(m_bd, sql.ToUTF8(), callback, data, &err_msg);
        if(rc != SQLITE_OK){
            show_error(wxString::FromUTF8(err_msg));
            sqlite3_free(err_msg);
            return false;
        }
        return true;
    }

    bool operator()(const wxString& sql, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr){return make_sql(sql, callback, data);}
    wxString operator+(const wxString& sql_part) const {return wxString::Format("SELECT * FROM %s %s", m_table_name, sql_part);}
};

#endif