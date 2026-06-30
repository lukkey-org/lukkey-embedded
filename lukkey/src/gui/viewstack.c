#include "viewstack.h"
#include <string.h>
#include <stdio.h>




viewstack_t viewstack;


void vs_init(void)
{
    viewstack.index = -1;
    memset(viewstack.stack, 0, sizeof(viewstack.stack));
}

bool vs_is_empty(void)
{
    return (viewstack.index < 0);
}

bool vs_is_full(void)
{
    return (viewstack.index >= (MAX_VIEW_DEPTH-1));
}

view_t *vs_top(void)
{
    if(vs_is_empty() == true)
        return NULL;

    return viewstack.stack[viewstack.index];
}

int vs_size(void)
{
    return (viewstack.index + 1);
}

bool vs_remove_if_exists(view_t *view)
{
    if(view == NULL)
        return false;

    for(int i = 0; i < vs_size(); i++)
    {
        if(viewstack.stack[i] == view)
        {
            vs_destroy(view);
            return true;
        }
    }
    return false;
}

bool vs_push(view_t *view)
{
    if(view == NULL)
        return false;

    if(vs_is_full() == true)
        return false;

    vs_remove_if_exists(view);

    view_t *top = vs_top();
    if(top != NULL)
    {
        if(top->onpause != NULL)
            top->onpause(top);
    }
    
    viewstack.stack[viewstack.index+1] = view;
    viewstack.stack[viewstack.index+1]->activity = true;
    viewstack.index += 1;
    
    top = vs_top();
    if(top->oncreate != NULL)
    {
        top->oncreate(top);
    }

    return true;
}

bool vs_push_with_data(view_t *view, void *data)
{
    if(view == NULL)
        return false;
    
    view->data = data;
    view->has_data = (data != NULL);

    return vs_push(view);
}

view_t *vs_pop(void)
{
    if(vs_is_empty() == true)
        return NULL;
    
    view_t *top = vs_top();
    if(top->onrelease != NULL)
        top->onrelease(top);
    viewstack.stack[viewstack.index] = NULL;
    viewstack.index -= 1;
    top->activity = false;

    top = vs_top();
    if(top != NULL)
    {
        if(top->onresume != NULL)
            top->onresume(top);
    }
    
    return vs_top();
}

bool vs_replace(view_t *view)
{
    if(view == NULL)
        return false;

    if(vs_is_empty() == true)
        return false;
    
    vs_pop();
    vs_push(view);

    return true;
}

bool vs_replace_with_data(view_t *view, void *data)
{
    if(view == NULL)
        return false;
    
    view->data = data;
    view->has_data = (data != NULL);

    return vs_replace(view);
}

void *vs_get_view_data(view_t *view)
{
    if(view != NULL&& view->has_data == true)
    {
        view->has_data = false;
        return view->data;
    }

    return NULL;
}

bool vs_is_activity(view_t *view)
{
    if(view != NULL&& view->activity == true)
    {
        return true;
    }

    return false;
}

bool vs_destroy(view_t *view)
{
    int offset, count;
    view_t *p;

    if(view == NULL)
        return false;

    if(vs_is_empty() == true)
        return false;
    for(int i = 0; i < vs_size(); i ++)
    {
        if(viewstack.stack[i] == view)
        {
            p = viewstack.stack[i];
            if(p == vs_top())
            {
                vs_pop();
            }
            else
            {
                if(p->onrelease != NULL)
                    p->onrelease(p);
                p->activity = false;

                count = vs_size() - i - 1;
                offset = i;
                for(int j = 0; j < count; j ++)
                {
                    viewstack.stack[j + offset] = viewstack.stack[j + offset + 1];
                }
                viewstack.index -= 1;
            }
            
            return true;
        }
    }

    return false;
}

bool vs_clear_and_push(view_t *view)
{
    if(view == NULL)
        return false;
    
    /* Pop all views and invoke onrelease */
    while(!vs_is_empty())
    {
        view_t *current = vs_top();
        if(current != NULL)
        {
            if(current->onrelease != NULL)
                current->onrelease(current);
            current->activity = false;
        }
        viewstack.stack[viewstack.index] = NULL;
        viewstack.index -= 1;
    }
    
    return vs_push(view);
}


bool vs_pop_to(view_t *target_view)
{
    if(target_view == NULL)
        return false;

    if(vs_is_empty())
        return false;

    int target_index = -1;
    for(int i = 0; i < vs_size(); i++)
    {
        if(viewstack.stack[i] == target_view)
        {
            target_index = i;
            break;
        }
    }
    
    if(target_index == -1)
        return false;

    if(target_index == viewstack.index)
        return true;

    while(viewstack.index > target_index)
    {
        view_t *current = vs_top();
        if(current != NULL)
        {
            if(current->onrelease != NULL)
                current->onrelease(current);
            current->activity = false;
        }
        viewstack.stack[viewstack.index] = NULL;
        viewstack.index -= 1;
    }
    
    view_t *target = vs_top();
    if(target != NULL)
    {
        if(target->onresume != NULL)
        {
            target->onresume(target);
        }
    }
    
    return true;
}

int vs_pop_count(int count)
{
    if(count <= 0)
        return 0;
        
    int popped = 0;
    while(popped < count && !vs_is_empty())
    {
        vs_pop();
        popped++;
    }
    
    return popped;
}

void vs_print_all_describes(void)
{
    VIEW_PRINT("viewstack size: %d\n", vs_size());
    for(int i = 0; i < vs_size(); i++)
    {
        VIEW_PRINT("viewstack[%d]: %s %s\n", i, viewstack.stack[i]->describe, viewstack.index == i ? "<-- TOP" : "");
    }
}
