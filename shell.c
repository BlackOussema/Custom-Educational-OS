#include "shell.h"
#include "fs.h"

// دالة مساعدة للتحقق من بداية النص (لأننا لا نملك string.h)
int startsWith(const char *pre, const char *str) {
    while (*pre) {
        if (*pre != *str) return 0;
        pre++;
        str++;
    }
    return 1;
}

void handle_shell(char* cmd) {
    kprint("\n", 0x0F);

    // 1. أمر عرض الملفات
    if (strcmp(cmd, "ls") == 0) {
        fs_list();
    } 
    // 2. أمر إنشاء ملف: touch filename
    else if (startsWith("touch ", cmd)) {
        fs_create(cmd + 6);
    }
    // 3. أمر الكتابة المشفرة: write filename content
    else if (startsWith("write ", cmd)) {
        char* filename = cmd + 6;
        char* content = 0;
        
        // البحث عن المسافة لفصل الاسم عن المحتوى
        for(int i = 0; filename[i] != '\0'; i++) {
            if(filename[i] == ' ') {
                filename[i] = '\0'; // إنهاء سلسلة الاسم
                content = &filename[i+1]; // بداية المحتوى
                break;
            }
        }
        
        if(content) {
            fs_write(filename, content);
        } else {
            kprint("Usage: write <file> <text>", 0x0C);
        }
    }
    // 4. أمر القراءة وفك التشفير: cat filename
    else if (startsWith("cat ", cmd)) {
        fs_read(cmd + 4);
    }
    // 5. أوامر عامة
    else if (strcmp(cmd, "help") == 0) {
        kprint("Commands: ls, touch, write, cat, clear, reboot", 0x0E);
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(cmd, "reboot") == 0) {
        outb(0x64, 0xFE);
    }
    else if (cmd[0] != '\0') {
        kprint("Unknown command: ", 0x0C);
        kprint(cmd, 0x0C);
    }

    kprint("\n(root@shield)-# ", 0x0A);
}
