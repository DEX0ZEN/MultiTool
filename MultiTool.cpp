#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <limits>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;
namespace fs = filesystem;

// ANSI escape codes
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

int get_terminal_width() {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
}

void center_text(const string& text, char pad_char = ' ', bool bold = false) {
    int width = get_terminal_width();
    int padding = (width - text.length()) / 2;
    if(padding < 0) padding = 0;
    
    cout << string(padding, pad_char);
    if(bold) cout << BOLD;
    cout << text;
    if(bold) cout << RESET;
}

void display_ascii_art() {
    vector<string> ascii_lines = {
        ",---,        ,---,. ,--,     ,--,  ",
        "  .'  .' `\\    ,'  .' | |'. \\   / .`|  ",
        ",---.'     \\ ,---.'   | ; \\ `\\ /' / ;  ",
        "|   |  .`\\  ||   |   .' `. \\  /  / .'  ",
        ":   : |  '  |:   :  |-,  \\  \\/  / ./   ",
        "|   ' '  ;  ::   |  ;/|   \\  \\.'  /    ",
        "'   | ;  .  ||   :   .'    \\  ;  ;     ",
        "|   | :  |  '|   |  |-,   / \\  \\  \\    ",
        "'   : | /  ; '   :  ;/|  ;  /\\  \\  \\   ",
        "|   | '` ,/  |   |    \\./__;  \\  ;  \\  ",
        ";   :  .'    |   :   .'|   : / \\  \\  ; ",
        "|   ,.'      |   | ,'  ;   |/   \\  ' | ",
        "'---'        `----'    `---'     `--`   "
    };

    cout << GREEN;
    for(const auto& line : ascii_lines) {
        center_text(line, ' ', true);
        cout << endl;
    }
    cout << RESET;
}

void clear_screen() {
    cout << "\033[2J\033[1;1H";
}

void draw_box(const string& title) {
    int width = min(get_terminal_width() - 4, 80);
    string border(width, '-');
    
    cout << YELLOW;
    center_text("+" + border + "+\n");
    center_text("|" + string(width, ' ') + "|\n");
    center_text("|");
    cout << string((width - title.length())/2 - 1, ' ');
    cout << BOLD << MAGENTA << " " << title << " " << RESET << YELLOW;
    cout << string((width - title.length())/2 - 1, ' ') << "|\n";
    center_text("|" + string(width, ' ') + "|\n");
    center_text("+" + border + "+\n");
    cout << RESET;
}

void press_enter_to_continue() {
    cout << YELLOW;
    center_text("Press Enter to continue...");
    cout << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

pair<int, string> process_line(const string& line, bool is_root) {
    int depth = 0;
    size_t pos = 0;

    size_t marker_pos = line.find("|-- ");
    if (marker_pos == string::npos) {
        marker_pos = line.find("`-- ");
    }

    if (marker_pos != string::npos) {
        string indentation = line.substr(0, marker_pos);
        while (indentation.size() >= 4) {
            depth++;
            indentation = indentation.substr(4);
        }
    }

    if (!is_root) depth += 1;

    string cleaned;
    if (marker_pos != string::npos) {
        cleaned = line.substr(marker_pos + 4);
        cleaned.erase(remove_if(cleaned.begin(), cleaned.end(), [](char c) {
            return string("-|` ").find(c) != string::npos;
        }), cleaned.end());
        
        size_t first = cleaned.find_first_not_of(' ');
        if (first != string::npos) {
            size_t last = cleaned.find_last_not_of(' ');
            bool has_slash = (last + 1 < cleaned.size() && cleaned[last + 1] == '/');
            cleaned = cleaned.substr(first, (has_slash ? last + 1 : last) - first + 1);
        }
    } else {
        cleaned = line;
        cleaned.erase(remove_if(cleaned.begin(), cleaned.end(), [](char c) {
            return string("-|` ").find(c) != string::npos;
        }), cleaned.end());
        
        size_t first = cleaned.find_first_not_of(' ');
        if (first != string::npos) {
            size_t last = cleaned.find_last_not_of('/');
            cleaned = (last == string::npos) ? "" : cleaned.substr(first, last - first + 1);
        }
    }

    return {depth, cleaned};
}

void create_directory_structure() {
    clear_screen();
    draw_box("Directory Tree Creator");
    
    vector<string> tree;
    string line;
    
    cout << CYAN << "\n";
    center_text("Enter directory tree (empty line to finish):\n");
    cout << "Example:\n";
    cout << "root/\n";
    cout << "|-- subdir1/\n";
    cout << "|   `-- file.txt\n";
    cout << "`-- subdir2/\n" << RESET;

    while(getline(cin, line) && !line.empty()) {
        tree.push_back(line);
    }

    if(tree.empty()) {
        cout << RED;
        center_text("No input detected!\n");
        cout << RESET;
        press_enter_to_continue();
        return;
    }

    cout << CYAN;
    center_text("Enter base directory path: ");
    cout << RESET;
    string base_dir;
    getline(cin, base_dir);

    try {
        fs::create_directories(base_dir);
        auto [root_depth, root_name] = process_line(tree[0], true);
        fs::path full_path = fs::path(base_dir) / root_name;
        fs::create_directories(full_path);

        stack<pair<fs::path, int>> dir_stack;
        dir_stack.push({full_path, -1});

        for(size_t i = 1; i < tree.size(); ++i) {
            auto [current_depth, entry] = process_line(tree[i], false);
            if(entry.empty()) continue;

            while(!dir_stack.empty() && dir_stack.top().second >= current_depth) {
                dir_stack.pop();
            }

            fs::path parent_path = dir_stack.top().first;
            fs::path current_path = parent_path / entry;

            bool is_dir = !entry.empty() && (entry.back() == '/' || entry.find('.') == string::npos);
            if(i + 1 < tree.size()) {
                auto [next_depth, _] = process_line(tree[i + 1], false);
                if(next_depth > current_depth) is_dir = true;
            }

            if(is_dir) {
                fs::create_directories(current_path);
                dir_stack.push({current_path, current_depth});
            } else {
                fs::create_directories(current_path.parent_path());
                ofstream(current_path).close();
            }
        }

        cout << GREEN << "\n";
        center_text("Structure successfully created at:\n");
        center_text(full_path.string() + "\n");

        cout << YELLOW;
        center_text("Delete created structure? (y/n): ");
        cout << RESET;
        char choice;
        cin >> choice;

        if(tolower(choice) == 'y') {
            fs::remove_all(full_path);
            cout << RED;
            center_text("Structure deleted!\n");
            cout << RESET;
        }
    } catch(const exception& e) {
        cout << RED;
        center_text("Error: " + string(e.what()) + "\n");
        cout << RESET;
    }
    press_enter_to_continue();
}

void generate_directory_tree(const fs::path& path, int depth = 0, bool is_last = false) {
    static const string indent = "    ";
    static const string branch = "|-- ";
    static const string last_branch = "`-- ";
    static const string vertical = "|   ";
    static const string space = "    ";

    if(depth == 0) cout << GREEN << path.filename() << RESET << "\n";
    
    vector<fs::path> entries;
    if(fs::is_directory(path)) {
        for(const auto& entry : fs::directory_iterator(path)) {
            entries.push_back(entry.path());
        }
        sort(entries.begin(), entries.end());
    }

    for(size_t i = 0; i < entries.size(); ++i) {
        bool last = (i == entries.size() - 1);
        const auto& entry = entries[i];
        
        for(int j = 0; j < depth; ++j) {
            cout << (j == depth-1 ? (last ? space : vertical) : vertical);
        }
        
        cout << (last ? last_branch : branch) 
             << (fs::is_directory(entry) ? GREEN : CYAN)
             << entry.filename().string() 
             << RESET << "\n";

        if(fs::is_directory(entry)) {
            generate_directory_tree(entry, depth + 1, last);
        }
    }
}

void directory_tree_generator() {
    clear_screen();
    draw_box("Directory Tree Generator");
    
    cout << CYAN << "\n";
    center_text("Enter directory path: ");
    cout << RESET;
    
    string path;
    getline(cin, path);
    
    try {
        if(!fs::exists(path)) {
            throw runtime_error("Path does not exist");
        }
        
        clear_screen();
        draw_box("Directory Structure");
        cout << "\n";
        generate_directory_tree(path);
        cout << "\n";
    } catch(const exception& e) {
        cout << RED;
        center_text("Error: " + string(e.what()) + "\n");
        cout << RESET;
    }
    press_enter_to_continue();
}

void display_main_menu() {
    clear_screen();
    display_ascii_art();
    
    cout << "\n\n";
    draw_box("Main Menu");
    
    cout << MAGENTA << "\n";
    center_text("1. Create Directory Structure\n");
    center_text("2. Generate Directory Tree\n");
    center_text("3. (Coming Soon)\n");
    center_text("4. (Coming Soon)\n");
    center_text("0. Exit\n\n");
    
    cout << YELLOW;
    center_text("Enter your choice: ");
    cout << RESET;
}

int main() {
    int choice;
    do {
        display_main_menu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(choice) {
            case 1:
                create_directory_structure();
                break;
            case 2:
                directory_tree_generator();
                break;
            case 3:
            case 4:
                clear_screen();
                draw_box("Under Development");
                cout << RED << "\n";
                center_text("This feature is coming soon!\n");
                cout << RESET;
                press_enter_to_continue();
                break;
            case 0:
                clear_screen();
                draw_box("Goodbye!");
                cout << GREEN << "\n";
                center_text("Thanks for using MultiTool!\n");
                cout << RESET;
                break;
            default:
                cout << RED;
                center_text("Invalid choice! Please try again.\n");
                cout << RESET;
                press_enter_to_continue();
        }
    } while(choice != 0);

    return 0;
}