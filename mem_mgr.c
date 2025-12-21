#include <stdint.h>

/* تعريف حدود الذاكرة */
// سنفترض أن الذاكرة المتاحة لنا تبدأ من العنوان 1 ميجابايت وتصل إلى 4 ميجابايت
#define MEM_START 0x100000 
#define MEM_END   0x400000 

static uint32_t current_ptr = MEM_START;

/**
 * دالة تخصيص الذاكرة (توازي malloc)
 * @param size: الحجم المطلوب بالبايت
 * @return: مؤشر (Pointer) لبداية المساحة المحجوزة، أو 0 إذا فنيت الذاكرة
 */
void* allocate(uint32_t size) {
    // التأكد من وجود مساحة كافية
    if (current_ptr + size > MEM_END) {
        return (void*)0; // الذاكرة ممتلئة (Out of Memory)
    }

    void* allocated_addr = (void*)current_ptr;
    
    // تحريك المؤشر للمساحة التالية
    current_ptr += size;

    // محاذاة الذاكرة (Memory Alignment) 
    // يفضل دائماً أن تكون العناوين مضاعفات لـ 4 لتحسين أداء المعالج
    if (current_ptr % 4 != 0) {
        current_ptr += (4 - (current_ptr % 4));
    }

    return allocated_addr;
}

/**
 * دالة إعادة ضبط الذاكرة (توازي free بشكل جماعي)
 * في هذا النوع من المديرين، نقوم بمسح كل شيء دفعة واحدة
 */
void reset_memory() {
    current_ptr = MEM_START;
}

/**
 * دالة لمعرفة المساحة المتبقية
 */
uint32_t get_free_space() {
    return MEM_END - current_ptr;
}
