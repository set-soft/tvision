#!/bin/sh
#
# This script starts xterm mapping the keyboard like the xterm
# shipped with XFree86 4.x does.
# I introduced some additional tweaks to:
# 1) Generate values for Tab key when used with modifiers CSI Flags T
# 2) Same for back space: CSI Flags K
# 3) The keypad 5, but is just for testing.
# 4) The delete key ever emmits an escape sequence. Will think about
#    changing it.
#
# Additionally it configures Backspace and Delete to help TV.
# I also added Alt+letter and Alt+Shift+letter because some XTerm fails
# to generate ESC+letter.
#
xterm -xrm "XTerm*VT100.Translations: #override \n\
 ~Shift ~Ctrl ~Alt <Key>Insert: string(0x1b) string(\"[2~\") \n\
  Shift ~Ctrl ~Alt <Key>Insert: string(0x1b) string(\"[2;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>Insert: string(0x1b) string(\"[2;3~\") \n\
  Shift ~Ctrl  Alt <Key>Insert: string(0x1b) string(\"[2;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>Insert: string(0x1b) string(\"[2;5~\") \n\
  Shift  Ctrl ~Alt <Key>Insert: string(0x1b) string(\"[2;6~\") \n\
 ~Shift  Ctrl  Alt <Key>Insert: string(0x1b) string(\"[2;7~\") \n\
  Shift  Ctrl  Alt <Key>Insert: string(0x1b) string(\"[2;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>Delete: string(0x1b) string(\"[3~\") \n\
  Shift ~Ctrl ~Alt <Key>Delete: string(0x1b) string(\"[3;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>Delete: string(0x1b) string(\"[3;3~\") \n\
  Shift ~Ctrl  Alt <Key>Delete: string(0x1b) string(\"[3;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>Delete: string(0x1b) string(\"[3;5~\") \n\
  Shift  Ctrl ~Alt <Key>Delete: string(0x1b) string(\"[3;6~\") \n\
 ~Shift  Ctrl  Alt <Key>Delete: string(0x1b) string(\"[3;7~\") \n\
  Shift  Ctrl  Alt <Key>Delete: string(0x1b) string(\"[3;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>Prior: string(0x1b) string(\"[5~\") \n\
  Shift ~Ctrl ~Alt <Key>Prior: string(0x1b) string(\"[5;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>Prior: string(0x1b) string(\"[5;3~\") \n\
  Shift ~Ctrl  Alt <Key>Prior: string(0x1b) string(\"[5;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>Prior: string(0x1b) string(\"[5;5~\") \n\
  Shift  Ctrl ~Alt <Key>Prior: string(0x1b) string(\"[5;6~\") \n\
 ~Shift  Ctrl  Alt <Key>Prior: string(0x1b) string(\"[5;7~\") \n\
  Shift  Ctrl  Alt <Key>Prior: string(0x1b) string(\"[5;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>Next: string(0x1b) string(\"[6~\") \n\
  Shift ~Ctrl ~Alt <Key>Next: string(0x1b) string(\"[6;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>Next: string(0x1b) string(\"[6;3~\") \n\
  Shift ~Ctrl  Alt <Key>Next: string(0x1b) string(\"[6;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>Next: string(0x1b) string(\"[6;5~\") \n\
  Shift  Ctrl ~Alt <Key>Next: string(0x1b) string(\"[6;6~\") \n\
 ~Shift  Ctrl  Alt <Key>Next: string(0x1b) string(\"[6;7~\") \n\
  Shift  Ctrl  Alt <Key>Next: string(0x1b) string(\"[6;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F5: string(0x1b) string(\"[15~\") \n\
  Shift ~Ctrl ~Alt <Key>F5: string(0x1b) string(\"[15;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F5: string(0x1b) string(\"[15;3~\") \n\
  Shift ~Ctrl  Alt <Key>F5: string(0x1b) string(\"[15;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F5: string(0x1b) string(\"[15;5~\") \n\
  Shift  Ctrl ~Alt <Key>F5: string(0x1b) string(\"[15;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F5: string(0x1b) string(\"[15;7~\") \n\
  Shift  Ctrl  Alt <Key>F5: string(0x1b) string(\"[15;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F6: string(0x1b) string(\"[16~\") \n\
  Shift ~Ctrl ~Alt <Key>F6: string(0x1b) string(\"[16;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F6: string(0x1b) string(\"[16;3~\") \n\
  Shift ~Ctrl  Alt <Key>F6: string(0x1b) string(\"[16;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F6: string(0x1b) string(\"[16;5~\") \n\
  Shift  Ctrl ~Alt <Key>F6: string(0x1b) string(\"[16;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F6: string(0x1b) string(\"[16;7~\") \n\
  Shift  Ctrl  Alt <Key>F6: string(0x1b) string(\"[16;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F7: string(0x1b) string(\"[18~\") \n\
  Shift ~Ctrl ~Alt <Key>F7: string(0x1b) string(\"[18;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F7: string(0x1b) string(\"[18;3~\") \n\
  Shift ~Ctrl  Alt <Key>F7: string(0x1b) string(\"[18;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F7: string(0x1b) string(\"[18;5~\") \n\
  Shift  Ctrl ~Alt <Key>F7: string(0x1b) string(\"[18;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F7: string(0x1b) string(\"[18;7~\") \n\
  Shift  Ctrl  Alt <Key>F7: string(0x1b) string(\"[18;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F8: string(0x1b) string(\"[19~\") \n\
  Shift ~Ctrl ~Alt <Key>F8: string(0x1b) string(\"[19;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F8: string(0x1b) string(\"[19;3~\") \n\
  Shift ~Ctrl  Alt <Key>F8: string(0x1b) string(\"[19;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F8: string(0x1b) string(\"[19;5~\") \n\
  Shift  Ctrl ~Alt <Key>F8: string(0x1b) string(\"[19;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F8: string(0x1b) string(\"[19;7~\") \n\
  Shift  Ctrl  Alt <Key>F8: string(0x1b) string(\"[19;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F9: string(0x1b) string(\"[20~\") \n\
  Shift ~Ctrl ~Alt <Key>F9: string(0x1b) string(\"[20;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F9: string(0x1b) string(\"[20;3~\") \n\
  Shift ~Ctrl  Alt <Key>F9: string(0x1b) string(\"[20;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F9: string(0x1b) string(\"[20;5~\") \n\
  Shift  Ctrl ~Alt <Key>F9: string(0x1b) string(\"[20;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F9: string(0x1b) string(\"[20;7~\") \n\
  Shift  Ctrl  Alt <Key>F9: string(0x1b) string(\"[20;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F10: string(0x1b) string(\"[21~\") \n\
  Shift ~Ctrl ~Alt <Key>F10: string(0x1b) string(\"[21;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F10: string(0x1b) string(\"[21;3~\") \n\
  Shift ~Ctrl  Alt <Key>F10: string(0x1b) string(\"[21;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F10: string(0x1b) string(\"[21;5~\") \n\
  Shift  Ctrl ~Alt <Key>F10: string(0x1b) string(\"[21;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F10: string(0x1b) string(\"[21;7~\") \n\
  Shift  Ctrl  Alt <Key>F10: string(0x1b) string(\"[21;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F11: string(0x1b) string(\"[23~\") \n\
  Shift ~Ctrl ~Alt <Key>F11: string(0x1b) string(\"[23;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F11: string(0x1b) string(\"[23;3~\") \n\
  Shift ~Ctrl  Alt <Key>F11: string(0x1b) string(\"[23;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F11: string(0x1b) string(\"[23;5~\") \n\
  Shift  Ctrl ~Alt <Key>F11: string(0x1b) string(\"[23;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F11: string(0x1b) string(\"[23;7~\") \n\
  Shift  Ctrl  Alt <Key>F11: string(0x1b) string(\"[23;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>F12: string(0x1b) string(\"[24~\") \n\
  Shift ~Ctrl ~Alt <Key>F12: string(0x1b) string(\"[24;2~\") \n\
 ~Shift ~Ctrl  Alt <Key>F12: string(0x1b) string(\"[24;3~\") \n\
  Shift ~Ctrl  Alt <Key>F12: string(0x1b) string(\"[24;4~\") \n\
 ~Shift  Ctrl ~Alt <Key>F12: string(0x1b) string(\"[24;5~\") \n\
  Shift  Ctrl ~Alt <Key>F12: string(0x1b) string(\"[24;6~\") \n\
 ~Shift  Ctrl  Alt <Key>F12: string(0x1b) string(\"[24;7~\") \n\
  Shift  Ctrl  Alt <Key>F12: string(0x1b) string(\"[24;8~\") \n\
 ~Shift ~Ctrl ~Alt <Key>Up: string(0x1b) string(\"OA\") \n\
  Shift ~Ctrl ~Alt <Key>Up: string(0x1b) string(\"[2A\") \n\
 ~Shift ~Ctrl  Alt <Key>Up: string(0x1b) string(\"[3A\") \n\
  Shift ~Ctrl  Alt <Key>Up: string(0x1b) string(\"[4A\") \n\
 ~Shift  Ctrl ~Alt <Key>Up: string(0x1b) string(\"[5A\") \n\
  Shift  Ctrl ~Alt <Key>Up: string(0x1b) string(\"[6A\") \n\
 ~Shift  Ctrl  Alt <Key>Up: string(0x1b) string(\"[7A\") \n\
  Shift  Ctrl  Alt <Key>Up: string(0x1b) string(\"[8A\") \n\
 ~Shift ~Ctrl ~Alt <Key>Down: string(0x1b) string(\"OB\") \n\
  Shift ~Ctrl ~Alt <Key>Down: string(0x1b) string(\"[2B\") \n\
 ~Shift ~Ctrl  Alt <Key>Down: string(0x1b) string(\"[3B\") \n\
  Shift ~Ctrl  Alt <Key>Down: string(0x1b) string(\"[4B\") \n\
 ~Shift  Ctrl ~Alt <Key>Down: string(0x1b) string(\"[5B\") \n\
  Shift  Ctrl ~Alt <Key>Down: string(0x1b) string(\"[6B\") \n\
 ~Shift  Ctrl  Alt <Key>Down: string(0x1b) string(\"[7B\") \n\
  Shift  Ctrl  Alt <Key>Down: string(0x1b) string(\"[8B\") \n\
 ~Shift ~Ctrl ~Alt <Key>Right: string(0x1b) string(\"OC\") \n\
  Shift ~Ctrl ~Alt <Key>Right: string(0x1b) string(\"[2C\") \n\
 ~Shift ~Ctrl  Alt <Key>Right: string(0x1b) string(\"[3C\") \n\
  Shift ~Ctrl  Alt <Key>Right: string(0x1b) string(\"[4C\") \n\
 ~Shift  Ctrl ~Alt <Key>Right: string(0x1b) string(\"[5C\") \n\
  Shift  Ctrl ~Alt <Key>Right: string(0x1b) string(\"[6C\") \n\
 ~Shift  Ctrl  Alt <Key>Right: string(0x1b) string(\"[7C\") \n\
  Shift  Ctrl  Alt <Key>Right: string(0x1b) string(\"[8C\") \n\
 ~Shift ~Ctrl ~Alt <Key>Left: string(0x1b) string(\"OD\") \n\
  Shift ~Ctrl ~Alt <Key>Left: string(0x1b) string(\"[2D\") \n\
 ~Shift ~Ctrl  Alt <Key>Left: string(0x1b) string(\"[3D\") \n\
  Shift ~Ctrl  Alt <Key>Left: string(0x1b) string(\"[4D\") \n\
 ~Shift  Ctrl ~Alt <Key>Left: string(0x1b) string(\"[5D\") \n\
  Shift  Ctrl ~Alt <Key>Left: string(0x1b) string(\"[6D\") \n\
 ~Shift  Ctrl  Alt <Key>Left: string(0x1b) string(\"[7D\") \n\
  Shift  Ctrl  Alt <Key>Left: string(0x1b) string(\"[8D\") \n\
 ~@Num_Lock ~Shift ~Ctrl ~Alt <Key>KP_5: string(0x1b) string(\"[E\") \n\
  @Num_Lock ~Shift ~Ctrl ~Alt <Key>KP_5: string(\"5\") \n\
 ~@Num_Lock Shift ~Ctrl ~Alt <Key>KP_5: string(\"5\") \n\
  @Num_Lock Shift ~Ctrl ~Alt <Key>KP_5: string(0x1b) string(\"[E\") \n\
 ~Shift ~Ctrl  Alt <Key>KP_5: string(0x1b) string(\"[3E\") \n\
  Shift ~Ctrl  Alt <Key>KP_5: string(0x1b) string(\"[4E\") \n\
 ~Shift  Ctrl ~Alt <Key>KP_5: string(0x1b) string(\"[5E\") \n\
  Shift  Ctrl ~Alt <Key>KP_5: string(0x1b) string(\"[6E\") \n\
 ~Shift  Ctrl  Alt <Key>KP_5: string(0x1b) string(\"[7E\") \n\
  Shift  Ctrl  Alt <Key>KP_5: string(0x1b) string(\"[8E\") \n\
 ~Shift ~Ctrl ~Alt <Key>End: string(0x1b) string(\"OF\") \n\
  Shift ~Ctrl ~Alt <Key>End: string(0x1b) string(\"[2F\") \n\
 ~Shift ~Ctrl  Alt <Key>End: string(0x1b) string(\"[3F\") \n\
  Shift ~Ctrl  Alt <Key>End: string(0x1b) string(\"[4F\") \n\
 ~Shift  Ctrl ~Alt <Key>End: string(0x1b) string(\"[5F\") \n\
  Shift  Ctrl ~Alt <Key>End: string(0x1b) string(\"[6F\") \n\
 ~Shift  Ctrl  Alt <Key>End: string(0x1b) string(\"[7F\") \n\
  Shift  Ctrl  Alt <Key>End: string(0x1b) string(\"[8F\") \n\
 ~Shift ~Ctrl ~Alt <Key>Home: string(0x1b) string(\"OH\") \n\
  Shift ~Ctrl ~Alt <Key>Home: string(0x1b) string(\"[2H\") \n\
 ~Shift ~Ctrl  Alt <Key>Home: string(0x1b) string(\"[3H\") \n\
  Shift ~Ctrl  Alt <Key>Home: string(0x1b) string(\"[4H\") \n\
 ~Shift  Ctrl ~Alt <Key>Home: string(0x1b) string(\"[5H\") \n\
  Shift  Ctrl ~Alt <Key>Home: string(0x1b) string(\"[6H\") \n\
 ~Shift  Ctrl  Alt <Key>Home: string(0x1b) string(\"[7H\") \n\
  Shift  Ctrl  Alt <Key>Home: string(0x1b) string(\"[8H\") \n\
 ~Shift ~Ctrl ~Alt <Key>BackSpace: string(\"\177\")\n\
  Shift ~Ctrl ~Alt <Key>BackSpace: string(0x1b) string(\"[2K\") \n\
 ~Shift ~Ctrl  Alt <Key>BackSpace: string(0x1b) string(\"[3K\") \n\
  Shift ~Ctrl  Alt <Key>BackSpace: string(0x1b) string(\"[4K\") \n\
 ~Shift  Ctrl ~Alt <Key>BackSpace: string(0x1b) string(\"[5K\") \n\
  Shift  Ctrl ~Alt <Key>BackSpace: string(0x1b) string(\"[6K\") \n\
 ~Shift  Ctrl  Alt <Key>BackSpace: string(0x1b) string(\"[7K\") \n\
  Shift  Ctrl  Alt <Key>BackSpace: string(0x1b) string(\"[8K\") \n\
  Shift ~Ctrl ~Alt <Key>Tab: string(0x1b) string(\"[2T\") \n\
 ~Shift ~Ctrl  Alt <Key>Tab: string(0x1b) string(\"[3T\") \n\
  Shift ~Ctrl  Alt <Key>Tab: string(0x1b) string(\"[4T\") \n\
 ~Shift  Ctrl ~Alt <Key>Tab: string(0x1b) string(\"[5T\") \n\
  Shift  Ctrl ~Alt <Key>Tab: string(0x1b) string(\"[6T\") \n\
 ~Shift  Ctrl  Alt <Key>Tab: string(0x1b) string(\"[7T\") \n\
  Shift  Ctrl  Alt <Key>Tab: string(0x1b) string(\"[8T\") \n\
 ~Shift ~Ctrl ~Alt <Key>F1: string(0x1b) string(\"OP\") \n\
  Shift ~Ctrl ~Alt <Key>F1: string(0x1b) string(\"2OP\") \n\
 ~Shift ~Ctrl  Alt <Key>F1: string(0x1b) string(\"3OP\") \n\
  Shift ~Ctrl  Alt <Key>F1: string(0x1b) string(\"4OP\") \n\
 ~Shift  Ctrl ~Alt <Key>F1: string(0x1b) string(\"5OP\") \n\
  Shift  Ctrl ~Alt <Key>F1: string(0x1b) string(\"6OP\") \n\
 ~Shift  Ctrl  Alt <Key>F1: string(0x1b) string(\"7OP\") \n\
  Shift  Ctrl  Alt <Key>F1: string(0x1b) string(\"8OP\") \n\
 ~Shift ~Ctrl ~Alt <Key>F2: string(0x1b) string(\"OQ\") \n\
  Shift ~Ctrl ~Alt <Key>F2: string(0x1b) string(\"2OQ\") \n\
 ~Shift ~Ctrl  Alt <Key>F2: string(0x1b) string(\"3OQ\") \n\
  Shift ~Ctrl  Alt <Key>F2: string(0x1b) string(\"4OQ\") \n\
 ~Shift  Ctrl ~Alt <Key>F2: string(0x1b) string(\"5OQ\") \n\
  Shift  Ctrl ~Alt <Key>F2: string(0x1b) string(\"6OQ\") \n\
 ~Shift  Ctrl  Alt <Key>F2: string(0x1b) string(\"7OQ\") \n\
  Shift  Ctrl  Alt <Key>F2: string(0x1b) string(\"8OQ\") \n\
 ~Shift ~Ctrl ~Alt <Key>F3: string(0x1b) string(\"OR\") \n\
  Shift ~Ctrl ~Alt <Key>F3: string(0x1b) string(\"2OR\") \n\
 ~Shift ~Ctrl  Alt <Key>F3: string(0x1b) string(\"3OR\") \n\
  Shift ~Ctrl  Alt <Key>F3: string(0x1b) string(\"4OR\") \n\
 ~Shift  Ctrl ~Alt <Key>F3: string(0x1b) string(\"5OR\") \n\
  Shift  Ctrl ~Alt <Key>F3: string(0x1b) string(\"6OR\") \n\
 ~Shift  Ctrl  Alt <Key>F3: string(0x1b) string(\"7OR\") \n\
  Shift  Ctrl  Alt <Key>F3: string(0x1b) string(\"8OR\") \n\
 ~Shift ~Ctrl ~Alt <Key>F4: string(0x1b) string(\"OS\") \n\
  Shift ~Ctrl ~Alt <Key>F4: string(0x1b) string(\"2OS\") \n\
 ~Shift ~Ctrl  Alt <Key>F4: string(0x1b) string(\"3OS\") \n\
  Shift ~Ctrl  Alt <Key>F4: string(0x1b) string(\"4OS\") \n\
 ~Shift  Ctrl ~Alt <Key>F4: string(0x1b) string(\"5OS\") \n\
  Shift  Ctrl ~Alt <Key>F4: string(0x1b) string(\"6OS\") \n\
 ~Shift  Ctrl  Alt <Key>F4: string(0x1b) string(\"7OS\") \n\
  Shift  Ctrl  Alt <Key>F4: string(0x1b) string(\"8OS\") \n\
 ~Shift ~Ctrl  Alt <Key>1: string(0x1b) string(\"1\") \n\
 ~Shift ~Ctrl  Alt <Key>2: string(0x1b) string(\"2\") \n\
 ~Shift ~Ctrl  Alt <Key>3: string(0x1b) string(\"3\") \n\
 ~Shift ~Ctrl  Alt <Key>4: string(0x1b) string(\"4\") \n\
 ~Shift ~Ctrl  Alt <Key>5: string(0x1b) string(\"5\") \n\
 ~Shift ~Ctrl  Alt <Key>6: string(0x1b) string(\"6\") \n\
 ~Shift ~Ctrl  Alt <Key>7: string(0x1b) string(\"7\") \n\
 ~Shift ~Ctrl  Alt <Key>8: string(0x1b) string(\"8\") \n\
 ~Shift ~Ctrl  Alt <Key>9: string(0x1b) string(\"9\") \n\
 ~Shift ~Ctrl  Alt <Key>0: string(0x1b) string(\"0\") \n\
 ~Shift ~Ctrl  Alt <Key>a: string(0x1b) string(\"a\") \n\
  Shift ~Ctrl  Alt <Key>a: string(0x1b) string(\"A\") \n\
 ~Shift ~Ctrl  Alt <Key>b: string(0x1b) string(\"b\") \n\
  Shift ~Ctrl  Alt <Key>b: string(0x1b) string(\"B\") \n\
 ~Shift ~Ctrl  Alt <Key>c: string(0x1b) string(\"c\") \n\
  Shift ~Ctrl  Alt <Key>c: string(0x1b) string(\"C\") \n\
 ~Shift ~Ctrl  Alt <Key>d: string(0x1b) string(\"d\") \n\
  Shift ~Ctrl  Alt <Key>d: string(0x1b) string(\"D\") \n\
 ~Shift ~Ctrl  Alt <Key>e: string(0x1b) string(\"e\") \n\
  Shift ~Ctrl  Alt <Key>e: string(0x1b) string(\"E\") \n\
 ~Shift ~Ctrl  Alt <Key>f: string(0x1b) string(\"f\") \n\
  Shift ~Ctrl  Alt <Key>f: string(0x1b) string(\"F\") \n\
 ~Shift ~Ctrl  Alt <Key>g: string(0x1b) string(\"g\") \n\
  Shift ~Ctrl  Alt <Key>g: string(0x1b) string(\"G\") \n\
 ~Shift ~Ctrl  Alt <Key>h: string(0x1b) string(\"h\") \n\
  Shift ~Ctrl  Alt <Key>h: string(0x1b) string(\"H\") \n\
 ~Shift ~Ctrl  Alt <Key>i: string(0x1b) string(\"i\") \n\
  Shift ~Ctrl  Alt <Key>i: string(0x1b) string(\"I\") \n\
 ~Shift ~Ctrl  Alt <Key>j: string(0x1b) string(\"j\") \n\
  Shift ~Ctrl  Alt <Key>j: string(0x1b) string(\"J\") \n\
 ~Shift ~Ctrl  Alt <Key>k: string(0x1b) string(\"k\") \n\
  Shift ~Ctrl  Alt <Key>k: string(0x1b) string(\"K\") \n\
 ~Shift ~Ctrl  Alt <Key>l: string(0x1b) string(\"l\") \n\
  Shift ~Ctrl  Alt <Key>l: string(0x1b) string(\"L\") \n\
 ~Shift ~Ctrl  Alt <Key>m: string(0x1b) string(\"m\") \n\
  Shift ~Ctrl  Alt <Key>m: string(0x1b) string(\"M\") \n\
 ~Shift ~Ctrl  Alt <Key>n: string(0x1b) string(\"n\") \n\
  Shift ~Ctrl  Alt <Key>n: string(0x1b) string(\"N\") \n\
 ~Shift ~Ctrl  Alt <Key>o: string(0x1b) string(\"o\") \n\
  Shift ~Ctrl  Alt <Key>o: string(0x1b) string(\"O\") \n\
 ~Shift ~Ctrl  Alt <Key>p: string(0x1b) string(\"p\") \n\
  Shift ~Ctrl  Alt <Key>p: string(0x1b) string(\"P\") \n\
 ~Shift ~Ctrl  Alt <Key>q: string(0x1b) string(\"q\") \n\
  Shift ~Ctrl  Alt <Key>q: string(0x1b) string(\"Q\") \n\
 ~Shift ~Ctrl  Alt <Key>r: string(0x1b) string(\"r\") \n\
  Shift ~Ctrl  Alt <Key>r: string(0x1b) string(\"R\") \n\
 ~Shift ~Ctrl  Alt <Key>s: string(0x1b) string(\"s\") \n\
  Shift ~Ctrl  Alt <Key>s: string(0x1b) string(\"S\") \n\
 ~Shift ~Ctrl  Alt <Key>t: string(0x1b) string(\"t\") \n\
  Shift ~Ctrl  Alt <Key>t: string(0x1b) string(\"T\") \n\
 ~Shift ~Ctrl  Alt <Key>u: string(0x1b) string(\"u\") \n\
  Shift ~Ctrl  Alt <Key>u: string(0x1b) string(\"U\") \n\
 ~Shift ~Ctrl  Alt <Key>v: string(0x1b) string(\"v\") \n\
  Shift ~Ctrl  Alt <Key>v: string(0x1b) string(\"V\") \n\
 ~Shift ~Ctrl  Alt <Key>w: string(0x1b) string(\"w\") \n\
  Shift ~Ctrl  Alt <Key>w: string(0x1b) string(\"W\") \n\
 ~Shift ~Ctrl  Alt <Key>x: string(0x1b) string(\"x\") \n\
  Shift ~Ctrl  Alt <Key>x: string(0x1b) string(\"X\") \n\
 ~Shift ~Ctrl  Alt <Key>y: string(0x1b) string(\"y\") \n\
  Shift ~Ctrl  Alt <Key>y: string(0x1b) string(\"Y\") \n\
 ~Shift ~Ctrl  Alt <Key>z: string(0x1b) string(\"z\") \n\
  Shift ~Ctrl  Alt <Key>z: string(0x1b) string(\"Z\") \n" \
-xrm "*metaSendsEscape: true" \
-xrm "*VT100*color0:  black" \
-xrm "*VT100*color1:  #a80000" \
-xrm "*VT100*color2:  #00A800" \
-xrm "*VT100*color3:  #A85400" \
-xrm "*VT100*color4:  #0000A8" \
-xrm "*VT100*color5:  #A800A8" \
-xrm "*VT100*color6:  #00A8A8" \
-xrm "*VT100*color7:  #A8A8A8" \
-xrm "*VT100*color8:  #545454" \
-xrm "*VT100*color9:  #FC5454" \
-xrm "*VT100*color10: #54FC54" \
-xrm "*VT100*color11: #FCFC54" \
-xrm "*VT100*color12: #5454FC" \
-xrm "*VT100*color13: #FC54FC" \
-xrm "*VT100*color14: #54FCFC" \
-xrm "*VT100*color15: white" \
-fn 10x20 \
-geometry 80x25
# Ctrl  <Key>Prior: string(0x1b) string(\"[5;5~\") \n\
#       <Key>Prior: string(0x1b) string(\"[5~\") \n \
# No auto-wraparound
# +aw
# 10x20 font
# -fn 10x20
# 80x25 size
# -geometry 80x25
# A default title, used in combination with -e
# -title 'XTerm Settings Example'
# Execute a command
# -e $1

