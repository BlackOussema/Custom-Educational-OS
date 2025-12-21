#ifndef FS_H
#define FS_H

#include <stdint.h>

#define SECTOR_SIZE 512
#define MAX_FILES 16
#define ROOT_LBA 100 // بداية نظام الملفات على القرص

typedef struct {
    char name[12];        // اسم الملف
    uint32_t size;       // حجم الملف بالبايت
    uint32_t first_block; // رقم أول قطاع يحتوي على البيانات
    uint8_t  type;        // 0 للملف، 1 للمجلد
    uint8_t  active;      // هل الملف موجود أم محذوف
} __attribute__((packed)) DirectoryEntry;

// وظائف النظام الأساسية
void fs_init();
void fs_create(char* name);
void fs_list();
void fs_write(char* name, char* data);

#endif
