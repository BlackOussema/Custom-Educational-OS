#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

// دوال الكرنل الأساسية
extern void kprint(const char* str, uint8_t color);
extern int strcmp(const char* s1, const char* s2);
extern void clear_screen();
extern void outb(uint16_t port, uint8_t val);

// دوال نظام الملفات
extern void fs_init();
extern void fs_create(char* name);
extern void fs_list();
extern void fs_write(char* name, char* content);
extern void fs_read(char* name);

// دالة الشل
void handle_shell(char* cmd);

#endif
