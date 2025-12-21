#include <stdint.h>

/* --- ثوابت النظام والذاكرة الرسومية --- */
#define VIDEO_MEM ((uint16_t*)0xB8000)
#define MAX_FILES 16
#define FILENAME_LEN 12
#define ROOT_LBA 100
#define CRYPTO_KEY 0xDE  // مفتاح التشفير XOR الخاص بك

/* --- هياكل بيانات نظام الملفات --- */
typedef struct {
    char name[FILENAME_LEN];
    uint32_t size;
    uint32_t first_block;
    uint8_t active;
} __attribute__((packed)) FileEntry;

/* --- متغيرات الحالة العامة --- */
uint32_t cursor = 0;
char cmd_buf[100];
int cmd_idx = 0;
FileEntry root_dir[MAX_FILES];

/* --- 1. دوال العتاد (I/O) --- */
void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

/* --- 2. مشغل القرص الصلب (ATA LBA) --- */
void disk_operation(uint32_t LBA, uint16_t* buffer, uint8_t command) {
    outb(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
    outb(0x1F5, (uint8_t)(LBA >> 16));
    outb(0x1F7, (command == 0) ? 0x20 : 0x30); 
    while (!(inb(0x1F7) & 0x08));
    for (int i = 0; i < 256; i++) {
        if (command == 0) buffer[i] = inw(0x1F0);
        else outw(0x1F0, buffer[i]);
    }
}

/* --- 3. إدارة الشاشة والنصوص --- */
void kprint(const char* str, uint8_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') cursor = ((cursor / 80) + 1) * 80;
        else VIDEO_MEM[cursor++] = (uint16_t)str[i] | (uint16_t)color << 8;
    }
}

void clear_screen() {
    for (int i = 0; i < 80 * 25; i++) VIDEO_MEM[i] = 0x0F20;
    cursor = 0;
}

void print_logo() {
    kprint("  ____      _                 ____  _     _      _     _ \n", 0x0B);
    kprint(" / ___|    | |__   ___ _ __  / ___|| |__ (_) ___| | __| |\n", 0x0B);
    kprint(" | |  _____| '_ \\ / _ \\ '__| \\___ \\| '_ \\| |/ _ \\ |/ _` |\n", 0x03);
    kprint(" | |___|_____| |_) |  __/ |    ___) | | | | |  __/ | (_| |\n", 0x03);
    kprint(" \\____|     |_.__/ \\___|_|   |____/|_| |_|_|\\___|_|\\__,_|\n", 0x0E);
    kprint("          >> TUNISIAN CYBER DEFENSE KERNEL <<\n\n", 0x0E);
}

/* --- 4. معالجة النصوص والمقارنة --- */
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int startsWith(const char* pre, const char* str) {
    while (*pre) { if (*pre != *str) return 0; pre++; str++; }
    return 1;
}

/* --- 5. لوحة المفاتيح والـ Shell --- */
char get_char() {
    static const char scancode_table[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };
    while(1) {
        if (inb(0x64) & 0x01) {
            uint8_t sc = inb(0x60);
            if (!(sc & 0x80) && sc < sizeof(scancode_table)) return scancode_table[sc];
        }
    }
}

void handle_shell(char* cmd) {
    kprint("\n", 0x0F);
    if (strcmp(cmd, "help") == 0) {
        kprint("Commands: ls, touch <name>, write <file> <text>, cat <file>, clear, reboot\n", 0x0E);
    }
    else if (startsWith("touch ", cmd)) {
        char* name = cmd + 6;
        for(int i=0; i<MAX_FILES; i++) if(!root_dir[i].active) {
            int j=0; while(name[j] && j<11) { root_dir[i].name[j]=name[j]; j++; }
            root_dir[i].name[j]='\0'; root_dir[i].active=1;
            root_dir[i].first_block = ROOT_LBA + 1 + i;
            disk_operation(ROOT_LBA, (uint16_t*)root_dir, 1);
            kprint("File created.\n", 0x0A); break;
        }
    }
    else if (startsWith("write ", cmd)) {
        char* file = cmd + 6; char* text = 0;
        for(int i=0; file[i]; i++) if(file[i]==' ') { file[i]='\0'; text=&file[i+1]; break; }
        if(text) {
            for(int i=0; i<MAX_FILES; i++) if(root_dir[i].active && strcmp(root_dir[i].name, file)==0) {
                char buf[512] = {0}; int len=0;
                while(text[len] && len < 511) { buf[len] = text[len] ^ CRYPTO_KEY; len++; }
                disk_operation(root_dir[i].first_block, (uint16_t*)buf, 1);
                kprint("Encrypted and saved.\n", 0x0A);
            }
        }
    }
    else if (startsWith("cat ", cmd)) {
        char* name = cmd + 4;
        for(int i=0; i<MAX_FILES; i++) if(root_dir[i].active && strcmp(root_dir[i].name, name)==0) {
            char buf[512] = {0}; disk_operation(root_dir[i].first_block, (uint16_t*)buf, 0);
            kprint("Content: ", 0x0B);
            for(int j=0; buf[j] && j<511; j++) {
                char decoded = buf[j] ^ CRYPTO_KEY;
                VIDEO_MEM[cursor++] = (uint16_t)decoded | 0x0F00;
            }
            kprint("\n", 0x0F);
        }
    }
    else if (strcmp(cmd, "ls") == 0) {
        for(int i=0; i<MAX_FILES; i++) if(root_dir[i].active) { 
            kprint("- ", 0x07); kprint(root_dir[i].name, 0x0F); kprint("\n", 0x0F); 
        }
    }
    else if (strcmp(cmd, "clear") == 0) { clear_screen(); print_logo(); }
    else if (strcmp(cmd, "reboot") == 0) outb(0x64, 0xFE);
    else if (cmd[0] != '\0') kprint("Unknown command.\n", 0x0C);

    kprint("\n(root@shield)-# ", 0x0A);
}

/* --- 6. الدالة الرئيسية (Kernel Main) --- */
void kernel_main() {
    clear_screen();
    disk_operation(ROOT_LBA, (uint16_t*)root_dir, 0); // تحميل الـ Root Directory
    print_logo();
    
    kprint("[SECURITY] ENTER ACCESS KEY: ", 0x0E);
    char pass[12]; int p_idx = 0;
    while(1) {
        char c = get_char();
        if(c == '\n') {
            pass[p_idx] = '\0';
            if(strcmp(pass, "tn2025") == 0) break;
            else { kprint("\n[!] ACCESS DENIED.", 0x0C); while(1); }
        } else if(p_idx < 11) { pass[p_idx++] = c; kprint("*", 0x0A); }
    }
    
    clear_screen();
    print_logo();
    kprint("System Ready. Hard Disk Initialized. Encryption XOR Active.\n", 0x0A);
    kprint("(root@shield)-# ", 0x0A);
    
    while(1) {
        char c = get_char();
        if(c == '\n') {
            cmd_buf[cmd_idx] = '\0';
            handle_shell(cmd_buf);
            cmd_idx = 0;
        } else if(cmd_idx < 99) {
            VIDEO_MEM[cursor++] = (uint16_t)c | 0x0F00;
            cmd_buf[cmd_idx++] = c;
        }
    }
}
