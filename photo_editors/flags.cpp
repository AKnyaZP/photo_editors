#include <iosfwd>

struct Flag {
    bool flag_success : 1;
    bool sl_par : 1;
    bool t_par : 1;
    bool dialog : 1;
    bool flag_for_save_dialog : 1;
    bool flag_for_tool : 1;
    bool if_image_is_open : 1;
};