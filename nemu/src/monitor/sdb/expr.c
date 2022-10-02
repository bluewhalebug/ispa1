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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

word_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256,TK_sixnum,TK_tennum,TK_reg,TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_NOT,DEREF

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[0-9,a-f]+",TK_sixnum},//shiliujinzhi
  {"[0-9]+",TK_tennum} ,    // shijinzhi
  {"\\$[a-z,0-9]{1,3}",TK_reg} ,  //reg
  {"\\(",'('}  ,           //zuokuohao
  {"\\)",')'}  ,           //youkuohao
  {"\\*",'*'}  ,          //muti
  {"\\/",'/'}  ,          //divi
  {"\\+", '+'},          // plus  
  {"\\-",'-'}  ,          //jian
  {"==", TK_EQ},        // equal
  {"!=",TK_NEQ}   ,           //not equal 
  {"&&",TK_AND}  ,            //and
  {"\\|\\|",TK_OR}   ,        //or
  {"!",TK_NOT}            //not

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;
        
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        

        switch (rules[i].token_type) {
          case 257: 
                   tokens[nr_token].type=257;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 258:
                   tokens[nr_token].type=258;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 259:
                   tokens[nr_token].type=259;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 260:
                   tokens[nr_token].type=260;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 261:
                   tokens[nr_token].type=261;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 262:
                   tokens[nr_token].type=262;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 263:
                   tokens[nr_token].type=263;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
                   break;
          case 264:
                   tokens[nr_token].type=264;        
                   break; 
          case '(':
                   tokens[nr_token].type='(';
                   break;
          case ')':
                   tokens[nr_token].type=')';    
                   break;     
          case '+':
                   tokens[nr_token].type='+';
                   break;
          case '-':
                   tokens[nr_token].type='-';
                   break;
          case '*':
                  if(tokens[nr_token-1].type==TK_tennum || tokens[nr_token-1].type==TK_sixnum || tokens[nr_token-1].type==TK_reg) tokens[nr_token].type='*';
                  else tokens[nr_token].type=DEREF;
                   break;
          case '/':
                   tokens[nr_token].type='/';
                   break;
          default:
          nr_token--;
          break;
        }
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


bool check_parentheses(int p, int q){
  if(tokens[p].type !='(' || tokens[q].type != ')') return false;
  int match=0;
  for(int i=p;i<=q;i++){
  if(tokens[i].type =='(') match++;
  if(tokens[i].type == ')') match--;
  if(match<=0 && i<q) return false;
  }
  if(match!=0) return false;
  return true;
  }
  
  
  int youxianji(int x){
  if(tokens[x].type=='+')return 1;
  if(tokens[x].type=='-')return 1;
  if(tokens[x].type=='*')return 2;
  if(tokens[x].type=='/')return 2;
  return(100);
  }
  
  int opfind(int p,int q){
  int tag=2;  
  int match=0;
  int op=0;
  for(int i=p;i<=q;i++){
  if(tokens[i].type =='('){
  match++;
  i++;
  while(true){
  if(tokens[i].type =='(') match++;
  if(tokens[i].type ==')') match--;
  i++;
  if(match==0) break;
  }
  if(i>q) break;
  }
  if(tokens[i].type=='+' || tokens[i].type=='-' || tokens[i].type=='*' || tokens[i].type=='/'){
  if(youxianji(i)<=tag){
  tag=youxianji(i);
  op=i;}
  }
  }
  return op;
  }
  
  
  
  
  
int eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
    return 0;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
     if(tokens[p].type==258){
       int n=0;
       sscanf(tokens[p].str,"%d",&n);
       return n;}
     if(tokens[p].type==257){
       int n=0;
       sscanf(tokens[p].str,"%x",&n);
       return n;
     }
     if(tokens[p].type==259){
     bool suc=true;
     return isa_reg_str2val(tokens[p].str,&suc);}
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int op=opfind(p,q);
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': /* ... */return val1-val2;
      case '*': /* ... */return val1*val2;
      case '/': /* ... */return val1/val2;
      default: assert(0);
    }
  }
  return 0;
}







word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;}
return eval(0,nr_token-1);}

  /* TODO: Insert codes to evaluate the expression. */
  
  
  
