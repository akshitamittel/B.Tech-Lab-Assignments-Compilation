#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/list.h>
#include<linux/slab.h>

struct birthday {
  int day;
  int month;
  int year;
  struct list_head list;
};

struct birthday birthday_list;

int link_init(void) {
  struct birthday *person, *ptr;
  int i, year = 1990;
  printk(KERN_INFO "initialize kernel module\n");
  INIT_LIST_HEAD(&birthday_list.list); 

  for(i = 0; i < 5; i++)
  {
    person = kmalloc(sizeof(*person), GFP_KERNEL);
    person->day = i + 1;
    person->month = ((i+1) * 5) % 12;
    person->year = year;
    year++;
    INIT_LIST_HEAD(&person->list);
    list_add_tail(&(person->list), &(birthday_list.list));
  }

  printk(KERN_INFO "traversing the list using list_for_each_entry()\n");

  list_for_each_entry(ptr, &birthday_list.list, list) {
    printk(KERN_INFO "Day: %d Month: %d Year: %d\n", ptr->day, ptr->month, ptr->year);
  }
  printk(KERN_INFO "\n");

  return 0;
}

void link_exit(void) {
  struct birthday *ptr, *next;
  printk(KERN_INFO "kernel module unloaded.\n");
  list_for_each_entry_safe(ptr, next, &birthday_list.list, list) {
    list_del(&ptr->list);
    kfree(ptr);
  }
}

module_init(link_init); 
module_exit(link_exit);

MODULE_LICENSE("GPL"); 
MODULE_DESCRIPTION("Linkedlist Module"); 
MODULE_AUTHOR("SGG");
