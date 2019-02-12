#define   BEGIN  while(1) {
#define   WEND    }

void  Exit(int, int);
void  screen2(),  write_tab(), build(int, char**), write_button();
int   mouse_click(), mouse_move(), play(int code);
void  _rectangle(int,int,int,int), write_player();
int   test_deal(), click_deal(), run(int);
void  write_ballnum(), write_pass();
void  show_time(char *), set(), set_msg(), reset();
void  save(), get_msg(), write_rou(), write_ptr(int), help_msg(), about_msg();
int   show_select(char *);
void  write_rb();
