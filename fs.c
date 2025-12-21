#include "fs.h"
#include "shell.h"

DirectoryEntry root_directory[MAX_FILES];

void fs_init() {
    // تحميل جدول الملفات من القرص عند الإقلاع
    disk_operation(ROOT_LBA, (uint16_t*)root_directory, 0); 
}

void fs_list() {
    kprint("\nNAME          SIZE    TYPE\n", 0x0E);
    kprint("--------------------------\n", 0x07);
    for(int i = 0; i < MAX_FILES; i++) {
        if(root_directory[i].active) {
            kprint(root_directory[i].name, 0x0F);
            kprint("    ", 0x0F);
            // هنا يمكنك إضافة دالة تحويل الرقم لنص لعرض الحجم
            kprint("FILE\n", 0x02);
        }
    }
}

void fs_create(char* name) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(!root_directory[i].active) {
            for(int j=0; j<11; j++) root_directory[i].name[j] = name[j];
            root_directory[i].active = 1;
            root_directory[i].size = 0;
            root_directory[i].first_block = ROOT_LBA + 1 + i;
            
            // حفظ التغييرات على القرص فوراً
            disk_operation(ROOT_LBA, (uint16_t*)root_directory, 1);
            kprint("\n[FS] File created and synced to disk.", 0x0A);
            return;
        }
    }
}
