#ifndef __VIEW_STACK_H
#define __VIEW_STACK_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>

#define VIEW_DEBUG 1
#if VIEW_DEBUG
    #define VIEW_PRINT(fmt, ...) printf("[VIEW] " fmt, ##__VA_ARGS__)
#else
    #define VIEW_PRINT(fmt, ...) do {} while(0)
#endif

#define MAX_VIEW_DEPTH          (32)

typedef struct view_t view_t;
struct view_t
{
    enum{ _PAGE,_DIALOG}type;
    char *describe;
    bool activity;
    bool has_data;
    void (*oncreate)(view_t *);
    void (*onpause)(view_t *);
    void (*onresume)(view_t *);
    void (*onrelease)(view_t *);
    void (*onnotify)(view_t *, void *);
    bool (*on_back_gesture)(view_t *);
    void *data;
};

typedef struct 
{
    view_t *stack[MAX_VIEW_DEPTH];
    int index;
}viewstack_t;

extern viewstack_t viewstack;


void vs_init(void);
bool vs_is_empty(void);
bool vs_is_full(void);
view_t *vs_top(void);
int vs_size(void);
bool vs_remove_if_exists(view_t *view);
bool vs_push(view_t *view);
bool vs_push_with_data(view_t *view, void *data);
view_t *vs_pop(void);
bool vs_replace(view_t *view);
bool vs_replace_with_data(view_t *view, void *data);
void *vs_get_view_data(view_t *view);
bool vs_is_activity(view_t *view);
bool vs_destroy(view_t *view);
bool vs_clear_and_push(view_t *view);
bool vs_pop_to(view_t *target_view);
int vs_pop_count(int count);
void vs_print_all_describes(void);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif

