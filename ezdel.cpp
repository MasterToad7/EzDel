#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fileapi.h>
#include <winbase.h>
#include <Lmcons.h>

bool check_if_exclude(std::string username);
bool check_if_admin(std::string username);
void create_admin_list();
void parse_users();
void delete_users();

bool global_setting_make_exclude_white_list = false;
bool global_setting_exclude_admin = false;
std::vector<std::string> global_setting_exclude_list;
std::vector<std::string> global_setting_delete_list;
std::vector<std::string> global_admin_list;

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) 
    {
        if (std::string(argv[i]) == "-ea")
        {
            global_setting_exclude_admin = true;
        } 
        else if (std::string(argv[i]) == "-mw") 
        {
            global_setting_make_exclude_white_list = true;
        }
        {
            global_setting_exclude_list.push_back(argv[i]);
        }
    }

    create_admin_list();
    parse_users();

    std::cout << "Do you want to proceed y/n:";
    std::string user_input;
    std::cin >> user_input;
    if (user_input == "y") 
    {
        std::cout << "Starting Deletion Process" << std::endl;
        delete_users();
    }
    else 
    {
        std::cout << "Canceling Deletion Process" << std::endl;
    }

    system("pause");

    return 0;
}

void create_admin_list()
{
    const char* command = "powershell.exe -Command \"Get-LocalGroupMember -Name Administrators | Select-Object -ExpandProperty name\"";

    FILE* command_pipe_output = _popen(command, "r");
    if (command_pipe_output == NULL)
    {
        throw std::runtime_error(std::string("Failed to check for admins"));
    }
    char line[128];
    while (fgets(line, sizeof(line) - 1, command_pipe_output) != NULL)
    {
        std::string temp = line;
        temp.erase(0, temp.find('\\') + 1);
        temp.resize(temp.length() - 1);
        global_admin_list.push_back(temp);
    }
}

bool check_if_exclude(std::string username)
{
    for (std::string& i : global_setting_exclude_list)
    {
        if (username == i)
        { 
            return true;
        }
    }
    return false;
}

bool check_if_admin(std::string username)
{
    for (std::string& admin_username : global_admin_list)
    {
        if (username == admin_username)
        {
            return true;
        }
    }

    return false;
}

void parse_users()
{
    std::string path = "C:\\Users\\";
    for (const auto& entry : std::filesystem::directory_iterator(path)) 
    {
        if (entry.is_directory()) 
        {
            DWORD attributes = GetFileAttributes(entry.path().c_str());
            if (!((attributes & FILE_ATTRIBUTE_HIDDEN) || (attributes & FILE_ATTRIBUTE_SYSTEM)))
            {
                TCHAR tcurrent_username[UNLEN + 1];
                DWORD current_username_len = UNLEN + 1;
                GetUserName((TCHAR*)tcurrent_username, &current_username_len);
                std::wstring wcurrent_username = tcurrent_username;
                std::string current_username = std::string(wcurrent_username.begin(), wcurrent_username.end());
                std::string username = entry.path().string().erase(0, 9);
                bool delete_status = true;

                if (username == "Public" || username == current_username)
                {
                    delete_status = false;
                }
                if (!global_setting_make_exclude_white_list && check_if_exclude(username))
                {
                    delete_status = false;
                }
                if (global_setting_make_exclude_white_list && !check_if_exclude(username))
                {
                    delete_status = false;
                }
                if (global_setting_exclude_admin && check_if_admin(username))
                {
                    delete_status = false;
                }
                if (delete_status == true)
                {
                    std::cout << username << " will be deleted" << std::endl;
                    global_setting_delete_list.push_back(entry.path().string());
                }
                else
                {
                    std::cout << username << " will be saved" << std::endl;
                }
            }
        }
    }
}

void delete_users()
{
    for (std::string& path : global_setting_delete_list)
    {
        std::cout << "Deleting " << path << std::endl;
        std::string command = "rmdir /s /q " + path;
        system(command.c_str());
    }
}