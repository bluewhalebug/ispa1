/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char exp[32];
  uint32_t value;

  /* TODO: Add more members if necessary */

} WP;


static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(char *exp){
if(free_==NULL)assert(0);
WP *p=free_;
free_=free_->next;
p->next=NULL;
bool suc=true;
strcpy(p->exp,exp);
p->value=expr(p->exp,&suc);
if(suc==false)assert(0);
if(head==NULL){
head=p;
p->next=NULL;}
else{
WP *q=head;
while(q->next!=NULL)q=q->next;
q->next=p;
p->next=NULL;}
return p;
}

void free_wp(WP *wp){
if(wp==head)head=head->next;
else{
WP *p=head;
while(p->next!=wp)p=p->next;
p->next=wp->next;
}
wp->next=free_;
free_=wp;
}


/*bool delete_wp(int x){
bool del=false;
if(x==0){
WP *p=head;
free_wp(p);
del=true;
}
else{
WP *p=head;
int j=0;
while(j<x && p!=NULL){
p=p->next;
j++;
}
free_wp(p);
del=true;
}
return del;
}*/

bool delete_wp(int x){
for(WP *p=head; p!=NULL; p=p->next){
if(p->NO==x){
free_wp(p);
return true;}
}
return false;
}
void print_wp(){
for(WP *p=head; p!=NULL; p=p->next)
printf("%d  %s  %d\n",p->NO,p->exp,p->value);
}

bool check_wp(){
bool check=false;
for(WP *p=head; p!=NULL; p=p->next){
int a=p->value;
bool suc=true;
int b=expr(p->exp,&suc);
if(a!=b){
printf("%d  %s  origin:%d new:%d\n",p->NO,p->exp,a,b);
p->value=b;
check=true;
}
}
return check;
}


















/* TODO: Implement the functionality of watchpoint */

