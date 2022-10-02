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

enum {
  TK_NOTYPE = 256,tennum=0,sixnum,reg,'(',')','*','/','+','-',TK_EQ,note,and,or,not

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
  {"[0-9]+",tennum} ,    // shijinzhi
  {"0x[0-9,a-f]+",sixnum},//shiliujinzhi
  {"\\$[a-z]{2,3}",reg} ,  //reg
  {"\\(",'('}  ,           //zuokuohao
  {"\\)",')'}  ,           //youkuohao
  {"\\*",'*'}  ,          //muti
  {"\\/",'/'}  ,          //divi
  {"\\+", '+'},          // plus  
  {"\\-",'-'}  ,          //jian
  {"==", TK_EQ},        // equal
  {"!=",note}   ,           //not equal 
  {"&&",and}  ,            //and
  {"\\|\\|",or}   ,        //or
  {"!",not}            //not

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
          case 0: 
                   nr_token++;
                   tokens[nr_token].type=tennum;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
          case 1:
                   nr_token++;
                   tokens[nr_token].type=sixnum;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
          case 2:
                   nr_token++;
                   tokens[nr_token].type=reg;
                   strncpy(tokens[nr_token].str,substr_start ,substr_len);
          case 3:
                   nr_token++;
                   tokens[nr_token].type='(';
          case 4:
                   nr_token++;
                   tokens[nr_token].type=')';
          case 5:
                   nr_token++;
                   tokens[nr_token].type='*';
          case 6:
                   nr_token++;
                   tokens[nr_token].type'/';
          case 7:
                   nr_token++;
                   tokens[nr_token].type='+';         
          case 8:
                   nr_token++;
                   tokens[nr_token].type='-';
          case 9:
                   nr_token++;
                   tokens[nr_token].type=TK_EQ;         
          case 10:
                   nr_token++;
                   tokens[nr_token].type=note;
          case 11:
                   nr_token++;
                   tokens[nr_token].type=and;
          case 12:
                   nr_token++;
                   tokens[nr_token].type=or;
          case 13:
                   nr_token++;
                   tokens[nr_token].type=not;
          default: TODO();
        }

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
  
  int opfind(int p,int q){
  int match=0;
  int tag=8;
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
  if(tokens[i].type <= tag && tokens[i].type >=5 ) tag=i;
  }
  return tag;
  }
  
  
  
  
  
eval(p, q) {
  if (p > q) {
    /* Bad expression */
    assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
     if(tokens[p].type==tennum){
       uint32_t n=0;
       sscanf(tokens[p].str,"%d",&n);
       return n;}
     if(tokens[p].type==sixnum){
       uint32_t n=0;
       sscanf(tokens[p].str,"%x",&n);
       return n;
     }
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    op=opfind(p,q);
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': /* ... */return val1-val2;
      case '*': /* ... */return val1*val2;
      case '/': /* ... */return val1/val2;
      default: assert(0);
    }
  }
}
  return 0;
}






word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
eval(0,strlen(tokens))
} 
  }

  /* TODO: Insert codes to evaluate the expression. */
  
  
  
